import streamlit as st

import pandas as pd
import numpy  as np
from sklearn import linear_model
from sklearn import metrics
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MinMaxScaler

from matplotlib import pyplot as plt
from matplotlib import cm
import seaborn as sns
import tensorflow as tf

import json
import pickle

models = []
with open("./models/m1.json", "r") as infile:
  models.append(json.load(infile))
with open("./models/m2.json", "r") as infile:
  models.append(json.load(infile))

def str_from_model(model):
  name = model['type']
  r2 = model['r2']
  rmse = model['rmse']
  return f'{name}: r2 - {r2:.5f}, rmse - {rmse:.3f}'

model = st.radio("Select model:", [str_from_model(i) for i in models]).split(':')[0]

smoking = 0
if st.checkbox("Is smoking"):
  smoking = 1
age = st.number_input(f"Enter your age:", min_value=0, format="%d")
bmi = st.number_input(f"Enter your body mass index:", min_value=0.0, format="%.2f")

df = pd.DataFrame({'is_smoker':[smoking], 'age':[age], 'bmi':[bmi]})
result = 0

if model == 'Linear':
  with open('./models/m1.pkl', 'rb') as infile:
    m1 = pickle.load(infile)
  result = m1.predict(df)
elif model == 'Neural':
  with open("./models/m2_Xscaler.pkl", "rb") as infile:
    scalerNormX = pickle.load(infile)
  with open("./models/m2_Yscaler.pkl", "rb") as infile:
    scalerNormY = pickle.load(infile)
  m2 = tf.keras.models.load_model('./models/m2.keras')
  tmp = scalerNormX.transform(df)
  tmp = m2.predict(tmp, verbose = 0)
  result = scalerNormY.inverse_transform(tmp)
else:
  st.error('Unknown model to use!')

st.success(f'Result: {result[0][0]:.3f}')

