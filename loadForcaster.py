from __future__ import annotations
import math, random, warnings, tkinter as tk
from pathlib import Path
import pandas as pd, joblib
from sklearn.pipeline import make_pipeline
from sklearn.ensemble import RandomForestRegressor
warnings.filterwarnings("ignore", category=FutureWarning)

#dataset paths
ROOT = Path(__file__).resolve().parent if "__file__" in globals() else Path.cwd()
ENERGY_CSV  = ROOT / "energy_dataset.csv"
WEATHER_CSV = ROOT / "weather_features.csv"
MODEL_PKL   = ROOT / "rf_load_model.pkl"

HI_F = [39.5, 42.6, 50.9, 62.3, 71.9, 80.2, 84.2, 82.7, 75.1, 64.3, 53.5, 43.4]
LO_F = [27.4, 29.4, 36.0, 45.2, 55.0, 64.4, 69.3, 68.2, 60.8, 50.1, 41.1, 31.7]
SUNRISE, SUNSET, PEAK_H = 6, 20, 15
COMPASS = ("N","NE","E","SE","S","SW","W","NW")
SKY_ICON = {"Sunny":"☀️","Partly Cloudy":"⛅","Cloudy":"☁️","Overcast":"🌥️","Night":"🌙"}
EVENT_ICON = {"Hurricane":"🌀","Tropical Storm":"🌧️","NorEaster":"❄️","Drought":"🔥"}
EVENT_PROB = {"Hurricane":{8:0.003,9:0.005,10:0.002},
              "Tropical Storm":{6:0.002,7:0.003,8:0.004,9:0.004,10:0.002},
              "NorEaster":{11:0.004,12:0.005,1:0.005,2:0.005,3:0.004},
              "Drought":{6:0.002,7:0.003,8:0.003}}
EVENT_DUR = {"Hurricane":(24,72),"Tropical Storm":(12,36),"NorEaster":(18,48),"Drought":(168,336)}

MONTH_STARTS = [1,32,60,91,121,152,182,213,244,274,305,335]
weekday = lambda d:("Mon","Tue","Wed","Thu","Fri","Sat","Sun")[(d-1)%7]

def month_from_doy(d:int)->int:
    for m,s in enumerate(MONTH_STARTS,1):
        if d < s:
            return m-1
    return 12

def month_day(d:int):
    months=[("Jan",31),("Feb",28),("Mar",31),("Apr",30),("May",31),("Jun",30),
            ("Jul",31),("Aug",31),("Sep",30),("Oct",31),("Nov",30),("Dec",31)]
    for name,days in months:
        if d<=days:
            return d,name
        d-=days
    return d,"Dec"
fmt_hour=lambda h:f"{(h%12 or 12):02d}:00 {'AM' if h<12 or h==24 else 'PM'}"


def daily_bounds(mon:int):
    low = LO_F[mon-1] + random.uniform(-3,3)
    hi  = HI_F[mon-1] + random.uniform(-3,3)
    if hi - low < 10:
        hi = low + 10
    return int(low), int(hi)

def diurnal(low:int, hi:int, h:int)->int:
    if h<=PEAK_H:
        frac=(h-SUNRISE)/(PEAK_H-SUNRISE)
        base=low+(hi-low)*math.sin(frac*math.pi/2)
    else:
        frac=(h-PEAK_H)/(24-(PEAK_H-SUNRISE))
        base=hi-(hi-low)*math.sin(frac*math.pi/2)
    return int(round(base+random.choice([-2,-1,0,1,2])))

def step_wind(v:int, idx:int, storm:bool):
    v=max(0,min(v+random.randint(-8 if storm else -3,8 if storm else 3),90 if storm else 40))
    return v,(idx+random.choice([-1,0,1]))%8

def step_sky(state: str, night: bool, over: bool):
    if night:
        return "Night"
    if over:
        return "Overcast"
    tbl = {
        "Sunny": [("Sunny", 0.7), ("Partly Cloudy", 0.25), ("Cloudy", 0.05)],
        "Partly Cloudy": [("Sunny", 0.2), ("Partly Cloudy", 0.6), ("Cloudy", 0.2)],
        "Cloudy": [("Partly Cloudy", 0.3), ("Cloudy", 0.6), ("Sunny", 0.1)],
        "Overcast": [("Cloudy", 0.5), ("Overcast", 0.5)],   # handle Overcast too
        "Night": [("Night", 1.0)]   # <- ADD THIS LINE
    }
    choices, probs = zip(*tbl[state])
    return random.choices(choices, probs)[0]


#training and loading model

def load_model():
    if MODEL_PKL.exists():
        return joblib.load(MODEL_PKL)
    print("Training RF model – first run…")
    e=pd.read_csv(ENERGY_CSV,parse_dates=["time"])
    e["time"]=pd.to_datetime(e["time"],utc=True).dt.tz_convert(None)
    e=e[["time","total load actual"]].rename(columns={"total load actual":"load"})

    w=pd.read_csv(WEATHER_CSV,parse_dates=["dt_iso"]).rename(columns={"dt_iso":"time"})
    w["time"]=pd.to_datetime(w["time"],utc=True).dt.tz_convert(None)
    w=(w.assign(temp_c=lambda d:d["temp"]-273.15,
                wind_mph=lambda d:d["wind_speed"]*2.23694,
                precip_mm=lambda d:d[["rain_1h","snow_3h"]].fillna(0).sum(axis=1))
         .groupby("time").agg({"temp_c":"mean","wind_mph":"mean","clouds_all":"mean","humidity":"mean","precip_mm":"mean"}).reset_index())

    df=pd.merge(e,w,on="time",how="inner").dropna()
    df["hour"],df["month"],df["dow"] = df["time"].dt.hour, df["time"].dt.month, df["time"].dt.dayofweek
    X=df.drop(columns=["load","time"]); y=df["load"]
    model=make_pipeline(RandomForestRegressor(n_estimators=200,min_samples_leaf=3,n_jobs=-1,random_state=42))
    model.fit(X,y); joblib.dump(model,MODEL_PKL)
    return model

MODEL = load_model()

# ────────────────── simulator ──────────────────
class Simulator:
    def __init__(self, root: tk.Tk):
        self.root=root
        tk.Label(root,text="NYC Weather + Grid Load",font=("Helvetica",16,"bold")).pack()
        self.emoji  = tk.Label(root,font=("Helvetica",48)); self.emoji.pack()
        self.info   = tk.Label(root,font=("Consolas",14),justify="left"); self.info.pack(pady=4)
        self.banner = tk.Label(root,font=("Helvetica",12)); self.banner.pack(pady=2)

        self.doy=1; self.leap=1; self.hour=1
        self.low,self.high=daily_bounds(month_from_doy(self.doy))
        self.temp=self.low; self.wind=10; self.dir=0; self.sky="Sunny"; self.event=None
        self.root.after(2000, self.tick)

    # ----- events -----
    def maybe_start_event(self, mon:int):
        for et,p in EVENT_PROB.items():
            if random.random() < p.get(mon,0):
                self.event={"type":et,"hrs":random.randint(*EVENT_DUR[et])}; break

    def event_fx(self):
        adj=0; precip=random.uniform(0,0.05); over=storm=False; desc=""; ico=""
        if self.event:
            self.event["hrs"]-=1
            if self.event["hrs"]<=0:
                self.event=None
            else:
                et=self.event["type"]
                if et=="Hurricane":       adj,precip,over,storm = 5, random.uniform(1,3), True, True
                elif et=="Tropical Storm":adj,precip,over,storm = 3, random.uniform(0.6,1.5), True, True
                elif et=="NorEaster":     adj,precip,over,storm = -10,random.uniform
                elif et=="NorEaster":     adj,precip,over,storm = -10,random.uniform(0.8,2), True, True
                elif et=="Drought":       adj,precip,over,storm = 4, 0.0, False, False
                desc=f"{et} (⏳{self.event['hrs']}h)"
                ico=EVENT_ICON[et]
        return adj, round(precip,2), over, storm, desc, ico

    def predict_load(self, precip, cloud_pct):
        row = pd.DataFrame({
            "temp_c": [(self.temp-32)*5/9],
            "wind_mph": [self.wind],
            "clouds_all": [cloud_pct],
            "humidity": [60],
            "precip_mm": [precip*25.4],
            "hour": [self.hour],
            "month": [month_from_doy(self.doy)],
            "dow": [(self.doy-1)%7]
        })
        return int(MODEL.predict(row)[0])

    def tick(self):
        mon = month_from_doy(self.doy)
        if self.hour == 1:
            self.low, self.high = daily_bounds(mon)
            if not self.event:
                self.maybe_start_event(mon)

        base = diurnal(self.low, self.high, self.hour)
        adj, precip, over, storm, desc, ico = self.event_fx()
        self.temp = max(min(base + adj, self.high + 15), self.low - 15)
        self.wind, self.dir = step_wind(self.wind, self.dir, storm)
        night = self.hour < SUNRISE or self.hour >= SUNSET
        self.sky = step_sky(self.sky, night, over)
        cloud_pct = {"Sunny":5, "Partly Cloudy":40, "Cloudy":75, "Overcast":100, "Night":0}[self.sky]
        load = self.predict_load(precip, cloud_pct)
        household_load = load / 30000 * 3

        d, mo = month_day(self.doy)
        self.emoji.config(text=ico if ico else SKY_ICON[self.sky])
        self.info.config(text=(
            f"{weekday(self.doy)} {mo} {d:02d}  {fmt_hour(self.hour)}\n"
            f"Temp {self.temp}°F  Wind {self.wind} mph {COMPASS[self.dir]}\n"
            f"Precip {precip:.2f} in/hr  Sky {self.sky}\n"
            f"Predicted load: {household_load:.2f} kW (3 homes)"
        ))
        self.banner.config(text=desc)

        # advance time
        self.hour += 1
        if self.hour == 25:
            self.hour = 1
            self.doy += 1
            if self.doy > 365:
                self.doy = 1

        self.root.after(2000, self.tick)


if __name__ == "__main__":
    root = tk.Tk()
    Simulator(root)
    root.mainloop()
