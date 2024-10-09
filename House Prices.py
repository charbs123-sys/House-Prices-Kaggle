import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import math
from sklearn.linear_model import LogisticRegression
from sklearn.ensemble import RandomForestClassifier
from scipy.stats import expon as sp_expon # instead of grid search we specify a distribution from which the hyperparameter values may be sampled from
from scipy.stats import randint as sp_randint
import sklearn.gaussian_process as gp
import ruff

val = int(-1)
print(val)
#import seaborn as sb

#Conducting Exploratory Data Analysis (EDA)

house_data = pd.read_csv("train.csv")

#print(house_data.info())
house_data.drop(columns=['Id'],inplace=True)
print(house_data)
#print(house_data.describe())
#print(house_data.isna().sum())
log_base = LogisticRegression()



