#!/usr/bin/env python

import json
import numpy as np
import tensorflow as tf

def get_data(name):
    data = json.loads(open('planarmouse5.json').read())
    return [data[name][str(i)] for i in range(len(data[name]))]

vertices = get_data('vertices')
# edges = get_data('edges')
faces = get_data('faces')
# normals = get_data('normals')

quads = [f for f in faces if len(f) == 4]
quads_a, quads_b, quads_c, quads_d = zip(*quads)

vertices_tf = tf.Variable(vertices, trainable=True)
quads_a_tf = tf.one_hot(quads_a, len(vertices))
quads_b_tf = tf.one_hot(quads_b, len(vertices))
quads_c_tf = tf.one_hot(quads_c, len(vertices))
quads_d_tf = tf.one_hot(quads_d, len(vertices))
vertices_a_tf = tf.matmul(quads_a_tf, vertices_tf)
vertices_b_tf = tf.matmul(quads_b_tf, vertices_tf)
vertices_c_tf = tf.matmul(quads_c_tf, vertices_tf)
vertices_d_tf = tf.matmul(quads_d_tf, vertices_tf)
edge_ab_tf = vertices_a_tf - vertices_b_tf
edge_bc_tf = vertices_b_tf - vertices_c_tf
edge_cd_tf = vertices_c_tf - vertices_d_tf
edge_da_tf = vertices_d_tf - vertices_a_tf
normal_a_tf = tf.linalg.cross(edge_ab_tf, edge_da_tf)
normal_b_tf = tf.linalg.cross(edge_bc_tf, edge_ab_tf)
normal_c_tf = tf.linalg.cross(edge_cd_tf, edge_bc_tf)
normal_d_tf = tf.linalg.cross(edge_da_tf, edge_cd_tf)
normal_sum_tf = normal_a_tf + normal_b_tf + normal_c_tf + normal_d_tf
normal_len_tf = tf.sqrt(tf.reduce_sum(tf.square(normal_sum_tf), axis=1))
normal_unit_tf = tf.divide(normal_sum_tf, tf.expand_dims(normal_len_tf, 1))
vertices_mean_tf = (vertices_a_tf + vertices_b_tf + vertices_c_tf + vertices_d_tf) / 4
diff_a_tf = vertices_a_tf - vertices_mean_tf
diff_b_tf = vertices_b_tf - vertices_mean_tf
diff_c_tf = vertices_c_tf - vertices_mean_tf
diff_d_tf = vertices_d_tf - vertices_mean_tf


'''
How to do this loss function:

    Calculate new normal:
        4-point:
            normal_a = unit((cross(A-B, D-A)))
            normal_b = unit((cross(B-C, A-B)))
            normal_c = unit((cross(C-D, B-C)))
            normal_d = unit((cross(D-A, C-D)))
            normal = (normal_a + normal_b + normal_c + normal_d) / 4

    Calculate planar loss:
        Mac
'''


if __name__ == '__main__':
    print('hi')
