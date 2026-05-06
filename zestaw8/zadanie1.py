import requests
import pandas as pd
import sys

# program wyszukujący wszyskie albumy danego wykonawcy 

def find_albums(artist_id):
    req = requests.get(f'https://api.discogs.com/artists/{artist_id}/releases')
    data = req.json()['releases']
    df = pd.DataFrame(data)
    albums = list(df['title'])
    for album in albums:
        print(album)


def main():
    
    if len(sys.argv) != 2:
        print("Missing required arguments: zadanie2.py <artist_id>")
        sys.exit(1)
    else:
        artist_id = sys.argv[1]
        find_albums(artist_id)

if __name__ == "__main__":
    main()
    
