#!/usr/bin/env python
# Figure out min and max angles between vertices that share an edge

import json
import numpy as np

def unit(v):
    return np.asarray(v) / np.sqrt(np.sum(np.square(v)))

def angle_between(u, v):
    return np.arccos(np.clip(np.dot(unit(u), unit(v)), -1.0, 1.0))

def get_data(name):
    data = json.loads(open('planarmouse5.json').read())
    return [data[name][str(i)] for i in range(len(data[name]))]

vertices = get_data('vertices')
edges = get_data('edges')
faces = get_data('faces')
normals = get_data('normals')

angles = []
for i, face_1 in enumerate(faces):
    for j, face_2 in enumerate(faces):
        if i == j:
            continue
        if len(set(face_1) & set(face_2)) > 1:
            angles.append(angle_between(normals[i], normals[j]))

# print(angles)
print('min angle', min(angles), min(angles) / np.pi * 180)
print('max angle', max(angles), max(angles) / np.pi * 180)

x, y, z = zip(*vertices)
print('x', min(x), max(x), max(x) - min(x))
print('y', min(y), max(y), max(y) - min(y))
print('z', min(z), max(z), max(z) - min(z))
