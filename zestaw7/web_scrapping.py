import pandas as pd 
import numpy
from bs4 import BeautifulSoup
import requests

#simple script for scrapping title, year and month of publishing of posts on www.dogmatykarnisty.pl

BASE_URL = "https://www.dogmatykarnisty.pl"

years = range(2012, 2027)
months = range(1, 13)

results = []

for year in years:
    for month in months:
        url = f"{BASE_URL}/{year}/{month:02d}/"
        page = requests.get(url)
        if page.status_code != 200:
            continue

        soup = BeautifulSoup(page.content, 'html.parser')
        for tag in soup.select("h2.penci-entry-title a"):
            title = tag.get_text(strip=True)
            link = tag.get("href", "")
            results.append({"title": title, "url": link, "year": year, "month": month})


df = pd.DataFrame(results)
df.to_csv('dogmaty.csv', index= False)