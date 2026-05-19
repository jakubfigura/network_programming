import requests
import sys
import time

# program sprawdzający czy muzycy z zadanego zbioru grali w jednym zespole 
HEADERS = {'User-Agent': 'NetworkProgrammingBandCheck/1.0'}
### łańcuch określający user agent 
BASE_URL = 'https://api.discogs.com'

def api_get(url):
    while True:
        resp = requests.get(url, headers=HEADERS)
        if resp.status_code == 429:
            print("Oczekiwanie..., przekroczono limit zapytań\n")
            # czekamy, w systuacjach, gdy przekroczyliśmy limit zapytań 
            time.sleep(60)
            continue
        return resp

def get_artist(artist_id):
    resp = api_get(f'{BASE_URL}/artists/{artist_id}')
    if resp.status_code == 404:
        print(f"Artysta o ID {artist_id} nie istnieje.", file=sys.stderr)
        sys.exit(1)
    if resp.status_code != 200:
        print(f"Błąd serwera {resp.status_code} dla ID {artist_id}", file=sys.stderr)
        sys.exit(1)
    return resp.json()


def find_bands_of_artist(artist_id):
    data = get_artist(artist_id)
    groups = data.get('groups', [])
    return [(g['id'], g['name']) for g in groups]


def get_band_members(band_id):
    data = get_artist(band_id)
    members = data.get('members', [])
    return {m['id'] for m in members}



def main():
    if len(sys.argv) < 3:
        print("Użycie: zadanie2.py <artist_id> <artist_id2> ... <artist_idN>\n" \
        "Podaj minimum dwa ID", file=sys.stderr)
        sys.exit(1)

    artist_ids = []
    for arg in sys.argv[1:]:
        try:
            artist_ids.append(int(arg))
        except ValueError:
            print(f"Nieprawidłowe ID: {arg}, podaj liczbę całkowitą", file=sys.stderr)
            sys.exit(1)

    unique_ids = list(dict.fromkeys(artist_ids))
    if len(unique_ids) < 2:
        print("Podaj przynajmniej dwa różne ID.", file=sys.stderr)
        sys.exit(1)

    if len(unique_ids) < len(artist_ids):
        duplicates = [aid for aid in artist_ids if artist_ids.count(aid) > 1]
        dup_str = ', '.join(str(d) for d in sorted(set(duplicates)))
        print(f"Uwaga: pominięto powtarzające się ID: {dup_str}")
    artist_ids = unique_ids

    artist_names = {}
    for aid in artist_ids:
        data = get_artist(aid)
        artist_names[aid] = data.get('name', str(aid))

    all_bands = {}
    for aid in artist_ids:
        for band_id, band_name in find_bands_of_artist(aid):
            all_bands[band_id] = band_name

    results = []
    for band_id, band_name in all_bands.items():
        members = get_band_members(band_id)
        matching = [aid for aid in artist_ids if aid in members]
        if len(matching) >= 2:
            results.append((band_name, matching))

    results.sort(key=lambda x: x[0])

    if not results:
        names = ', '.join(artist_names[aid] for aid in artist_ids)
        print(f"Podani artyści: {names} nie grali razem w żadnym zespole.")
    else:
        for band_name, matching in results:
            names = ', '.join(artist_names[aid] for aid in matching)
            print(f"Artyści: {names} grali razem w zespole: {band_name}")

        artists_in_common = {aid for _, matching in results for aid in matching}
        excluded_artist = [artist_names[aid] for aid in artist_ids if aid not in artists_in_common]
        if excluded_artist:
            print(f"Nie grał z pozostałymi w żadnym zespole: {', '.join(excluded_artist)}")


if __name__ == "__main__":
    main()
