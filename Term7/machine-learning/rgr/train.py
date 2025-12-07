#!/bin/python

import pandas as pd
import numpy  as np
from sklearn import linear_model
from sklearn import metrics
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MinMaxScaler

#%matplotlib inline
from matplotlib import pyplot as plt
from matplotlib import cm
import seaborn as sns
import tensorflow as tf

import pickle
import json

def get_metrics(y_test, y_pred):
    mae = metrics.mean_absolute_error(y_test, y_pred)
    mse = metrics.mean_squared_error(y_test, y_pred)
    rmse = np.sqrt(mse)
    r2 = metrics.r2_score(y_test, y_pred)
    return r2, rmse

df = pd.read_csv('./datasets/insurance.csv', sep=',', header=0)

features = ['is_smoker', 'age', 'bmi']
target   = ['charges']

dfX = df[features]
dfY = df[target]

scalerNormX = MinMaxScaler()
scalerNormY = MinMaxScaler()

scalerNormX.fit(dfX)
scalerNormY.fit(dfY)

dfXNorm = pd.DataFrame (
  data    = scalerNormX.transform(dfX),
  columns = dfX.columns,
  index   = dfX.index
)
dfYNorm = pd.DataFrame (
  data    = scalerNormY.transform(dfY),
  columns = dfY.columns,
  index   = dfY.index
)

xTrain, xTest, yTrain, yTest = train_test_split(
    dfX,
    dfY,

    test_size = 0.3,
    shuffle = True
)

xNormTrain = pd.DataFrame (
  data    = scalerNormX.transform(xTrain),
  columns = xTrain.columns,
  index   = xTrain.index
)
xNormTest = pd.DataFrame (
  data    = scalerNormX.transform(xTest),
  columns = xTest.columns,
  index   = xTest.index
)
yNormTrain = pd.DataFrame (
  data    = scalerNormY.transform(yTrain),
  columns = yTrain.columns,
  index   = yTrain.index
)
yNormTest = pd.DataFrame (
  data    = scalerNormY.transform(yTest),
  columns = yTest.columns,
  index   = yTest.index
)

m1 = linear_model.LinearRegression()
m1.fit(xTrain, yTrain)

m1_yPred = m1.predict(xTest)
m1_r2, m1_rmse = get_metrics(yTest, m1_yPred)

m2 = tf.keras.models.Sequential()
m2.add(tf.keras.layers.Input(shape = (len(features),)))
m2.add(tf.keras.layers.Dense(units = 1, activation = None))
fLoss = tf.keras.losses.MeanSquaredError()
fOptimizer = tf.keras.optimizers.Adam(learning_rate = 0.01)
m2.compile(loss = fLoss, optimizer = fOptimizer)
m2.fit(xNormTrain, yNormTrain,
    validation_data=(
        xNormTest,
        yNormTest),
    epochs=100,
    verbose=0
)

m2_yNormPred = m2.predict(xNormTest, verbose = 0)
m2_r2, m2_rmse = get_metrics(yTest, scalerNormY.inverse_transform(m2_yNormPred))

m1_json = {'type':'Linear', 'normalized':False, 'features':features, 'target':target, 'r2':m1_r2, 'rmse':m1_rmse}
m2_json = {'type':'Neural', 'normalized':True, 'features':features, 'target':target, 'r2':m2_r2, 'rmse':m2_rmse}

with open("./models/m1.json", "w") as outfile:
    json.dump(m1_json, outfile)
with open("./models/m2.json", "w") as outfile:
    json.dump(m2_json, outfile)

with open("./models/m2_Xscaler.pkl", "wb") as outfile:
    pickle.dump(scalerNormX, outfile)
with open("./models/m2_Yscaler.pkl", "wb") as outfile:
    pickle.dump(scalerNormY, outfile)


with open("./models/m1.pkl", "wb") as outfile:
    pickle.dump(m1, outfile)
m2.save("./models/m2.keras")

