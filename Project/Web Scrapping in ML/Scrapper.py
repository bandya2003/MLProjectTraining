from bs4 import BeautifulSoup
import pandas as p
import requests

base_url="https://www.oyorooms.com/hotels-in-"
city=input("Enter your city:").lower()
page_no=1
d=p.DataFrame()
names=[]
rating=[]
ratings=[]
remark=[]
while page_no<=10:
    connect=requests.get(base_url+city+"/?page="+str(page_no))
    soup=BeautifulSoup(connect.text,"html.parser")
    for i in soup.findAll("h3",{"class":"listingHotelDescription__hotelName"}):
        names.append(i.text)
    for j in soup.findAll("div",{"class":"hotelRating"}):
        f=j.text.split("(")
        if "NEW" in f:
            rating.append(0.0)
            ratings.append(0)
            remark.append("No remark")
        else:
            rating.append(float(f[0].strip()))
            ratings.append(f[1][:4].split()[0])
            remark.append(f[1].split(")")[1][1:])
    page_no+=1
d["Hotel Name"]=names
d["Rating"]=rating
d["Ratings"]=ratings
d["Remark"]=remark
d.sort_values(by="Rating",ascending=False,inplace=True)
d.reset_index(inplace=True)
d.drop("index",1,inplace=True)
d.to_csv("File.csv")