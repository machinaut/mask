#!/usr/bin/env python
# thing I copy-paste into blender

import json
m = bpy.data.meshes['Cylinder']
vertices = {v.index: (v.co.x, v.co.y, v.co.z) for v in m.vertices}
edges = {e.index: tuple(e.vertices) for e in m.edges}
faces = {p.index: tuple(p.vertices) for p in m.polygons}
normals = {p.index: tuple(p.normal) for p in m.polygons}
data = dict(vertices=vertices, edges=edges, faces=faces, normals=normals)
with open('/tmp/planarmouse.json', 'w') as f:
    f.write(json.dumps(data))
