#!/usr/bin/python3
# -*- coding: UTF-8 -*-

'''
Aplikacja WSGI implementująca najważniejsze części opisywanej na wykładzie
usługi REST dającej dostęp do bazy z danymi osób.

Uwaga: kod dydaktyczny bez pełnej obsługi błędów i sytuacji nadzwyczajnych.

Aplikacja nie potrafi sama stworzyć swojej bazy danych, trzeba to zrobić
przed jej uruchomieniem. Skrypt rest_webapp.sh pokazuje jak.
'''

plik_bazy = './osoby.sqlite'

import re, sqlite3
from urllib.parse import parse_qs

class RestApp:
    table = None   # nazwa tabeli SQL, ustawiana w klasach pochodnych
    path  = None   # bazowa ścieżka URL '/osoby' lub '/psy'
    search_fields = []  # pola dozwolone w /search, ustawiane w klasach pochodnych

    def __init__(self, environment, start_response):
        self.env = environment
        self.start_response = start_response
        self.status = '200 OK'
        self.headers = [ ('Content-Type', 'text/html; charset=UTF-8') ]
        self.content = b''

    def __iter__(self):
        try:
            self.route()
        except sqlite3.Error as e:
            s = 'SQLite error: ' + str(e)
            self.failure('500 Internal Server Error', s)
        n = len(self.content)
        self.headers.append( ('Content-Length', str(n)) )
        self.start_response(self.status, self.headers)
        yield self.content

    def failure(self, status, detail = None):
        self.status = status
        s = '<html>\n<head>\n<title>' + status + '</title>\n</head>\n'
        s += '<body>\n<h1>' + status + '</h1>\n'
        if detail is not None:
            s += '<p>' + detail + '</p>\n'
        s += '</body>\n</html>\n'
        self.content = s.encode('UTF-8')

    def route(self):
        if self.env['PATH_INFO'] == self.path:
            self.handle_table()
            return
        if self.env['PATH_INFO'] == self.path + '/search':
            self.handle_search()
            return
        m = re.search('^' + self.path + '/(?P<id>[0-9]+)$', self.env['PATH_INFO'])
        if m is not None:
            self.handle_item(m.group('id'))
            return
        self.failure('404 Not Found')

    def handle_table(self):
        if self.env['REQUEST_METHOD'] == 'GET':
            colnames, rows = self.sql_select()
            self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'POST':
            colnames, vals = self.read_tsv()
            q = 'INSERT INTO ' + self.table + ' (' + ', '.join(colnames) + ') VALUES ('
            q += ', '.join(['?' for _ in vals]) + ')'
            id = self.sql_modify(q, vals)
            colnames, rows = self.sql_select(id)
            self.send_rows(colnames, rows)
        else:
            self.failure('501 Not Implemented')

    def handle_item(self, id):
        if self.env['REQUEST_METHOD'] == 'GET':
            colnames, rows = self.sql_select(id)
            if len(rows) == 0:
                self.failure('404 Not Found')
            else:
                self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'PUT':
            colnames, vals = self.read_tsv()
            q = 'UPDATE ' + self.table + ' SET '
            q += ', '.join([c + ' = ?' for c in colnames])
            q += ' WHERE id = ' + str(id)
            self.sql_modify(q, vals)
            colnames, rows = self.sql_select(id)
            self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'DELETE':
            q = 'DELETE FROM ' + self.table + ' WHERE id = ' + str(id)
            self.sql_modify(q)
        else:
            self.failure('501 Not Implemented')

    def handle_search(self):
        if self.env['REQUEST_METHOD'] != 'GET':
            self.failure('501 Not Implemented')
            return
        params = parse_qs(self.env.get('QUERY_STRING', ''))

        conditions = []
        vals = []
        for field in self.search_fields:
            if field in params:
                conditions.append(field + ' = ?')
                vals.append(params[field][0])

        if not conditions:
            self.failure('400 Bad Request',
                'Podaj parametr: ' + ' i/lub '.join(self.search_fields))
            return

        conn = sqlite3.connect(plik_bazy)
        crsr = conn.cursor()
        query = 'SELECT * FROM ' + self.table + ' WHERE ' + ' AND '.join(conditions)
        crsr.execute(query, vals)
        colnames = [d[0] for d in crsr.description]
        rows = crsr.fetchall()
        crsr.close()
        conn.close()
        self.send_rows(colnames, rows)

    def read_tsv(self):
        f = self.env['wsgi.input']
        n = int(self.env['CONTENT_LENGTH'])
        raw_bytes = f.read(n)
        lines = raw_bytes.decode('UTF-8').splitlines()
        colnames = lines[0].split('\t')
        vals = lines[1].split('\t')
        return colnames, vals

    def send_rows(self, colnames, rows):
        s = '\t'.join(colnames) + '\n'
        for row in rows:
            s += '\t'.join([str(val) for val in row]) + '\n'
        self.content = s.encode('UTF-8')
        self.headers = [ ('Content-Type',
                'text/tab-separated-values; charset=UTF-8') ]

    def sql_select(self, id = None):
        conn = sqlite3.connect(plik_bazy)
        crsr = conn.cursor()
        query = 'SELECT * FROM ' + self.table
        if id is not None:
            query += ' WHERE id = ' + str(id)
        crsr.execute(query)
        colnames = [ d[0] for d in crsr.description ]
        rows = crsr.fetchall()
        crsr.close()
        conn.close()
        return colnames, rows

    def sql_modify(self, query, params = None):
        conn = sqlite3.connect(plik_bazy)
        crsr = conn.cursor()
        # pragma foreing_keys 
        crsr.execute('PRAGMA foreign_keys = ON')
        if params is None:
            crsr.execute(query)
        else:
            crsr.execute(query, params)
        rowid = crsr.lastrowid
        crsr.close()
        conn.commit()
        conn.close()
        return rowid


class OsobyApp(RestApp):
    table = 'osoby'
    path  = '/osoby'
    search_fields = ['imie', 'nazwisko']


class PsyApp(RestApp):
    table = 'psy'
    path  = '/psy'
    search_fields = ['imie', 'rasa']

# object_router pozwala przekazać odpowiedni obiekt do klasy RestAPP
def object_router(environment, start_response):
    path = environment.get('PATH_INFO', '')
    if path.startswith('/psy'):
        return PsyApp(environment, start_response)
    return OsobyApp(environment, start_response)


if __name__ == '__main__':
    from wsgiref.simple_server import make_server
    port = 8000
    httpd = make_server('', port, object_router)
    print('Listening on port %i, press ^C to stop.' % port)
    httpd.serve_forever()
