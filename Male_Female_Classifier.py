from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier
from sklearn.neural_network import MLPClassifier
import numpy as np
from sklearn.metrics import accuracy_score
clf = DecisionTreeClassifier()
clf1=RandomForestClassifier()
clf2=AdaBoostClassifier()
clf3=MLPClassifier()
X = [[181, 80, 44], [177, 70, 43], [160, 60, 38], [154, 54, 37], [166, 65, 40],
     [190, 90, 47], [175, 64, 39],
     [177, 70, 40], [159, 55, 37], [171, 75, 42], [181, 85, 43]]

Y = ['male', 'male', 'female', 'female', 'male', 'male', 'female', 'female',
     'female', 'male', 'male']
clf.fit(X,Y)
clf1.fit(X,Y)
clf2.fit(X,Y)
clf3.fit(X,Y)
p=clf.predict(X)
p1=clf1.predict(X)
p2=clf2.predict(X)
p3=clf3.predict(X)
acc=accuracy_score(Y,p)
acc1=accuracy_score(Y,p1)
acc2=accuracy_score(Y,p2)
acc3=accuracy_score(Y,p3)
print(np.array(p)==np.array(Y))
print(np.array(p1)==np.array(Y))
print(np.array(p2)==np.array(Y))
print(np.array(p3)==np.array(Y))
###[ True  True  True  True  True  True  True  True  True  True  True]1
####[ True  True  True  True  True  True  True  True  True  True  True]1
###[ True  True  True  True  True  True  True  True  True  True  True]1
#[False False  True  True False False  True  True  True False False]0.45
