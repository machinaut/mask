#!/usr/bin/env python


class Segment:
    def __init__(self, offset, length, in0, in1, out0, out1, sign, twin):
        self.offset = offset
        self.length = length
        self.in0 = in0
        self.in1 = in1 if in1 else in0
        self.out0 = out0
        self.out1 = out1 if out1 else out0
        self.sign = sign
        self.twin = twin
        if self.sign > 0:
            self.start = self.offset
            self.end = self.offset + self.length - 1
        else:
            self.start = self.offset + self.length - 1
            self.end = self.offset


class Pixel:
    def __init__(self, i=0, idx=0, seg=0,
                 next_l_i=255, next_l_idx=0, next_l_seg=0,
                 next_r_i=255, next_r_idx=0, next_r_seg=0,
                 prev_l_i=255, prev_l_idx=0, prev_l_seg=0,
                 prev_r_i=255, prev_r_idx=0, prev_r_seg=0):
        self.r = self.g = self.b = 0
        self.i = i
        self.idx = idx
        self.seg = seg
        self.next_l_i = next_l_i
        self.next_l_idx = next_l_idx
        self.next_l_seg = next_l_seg
        self.next_r_i = next_r_i
        self.next_r_idx = next_r_idx
        self.next_r_seg = next_r_seg
        self.prev_l_i = prev_l_i
        self.prev_l_idx = prev_l_idx
        self.prev_l_seg = prev_l_seg
        self.prev_r_i = prev_r_i
        self.prev_r_idx = prev_r_idx
        self.prev_r_seg = prev_r_seg

    def __str__(self):
        return '{% 4d, % 4d, % 4d, % 4d, % 4d, 0, 0, 0}, // seg % 2d' % (
            self.next_l_i, self.next_r_i, self.prev_l_i, self.prev_r_i,
            self.i, self.seg)


# // offset length in0 in1 out0 out1 sign twin
segments = '''
    { -1, 0, 0, 0, 0, 0, 0, 0},  // sentinel
    {  0, 5, 0, 0, 2, 0, 1,18},  // 1
    {  5,10, 1, 0, 3, 0, 1,17},  // 2
    { 15, 8, 2, 0, 4, 0, 1,16},  // 3
    { 23, 7, 3, 0,14, 0, 1,15},  // 4
    { 30, 5,14, 0, 6, 0, 1,13},  // 5
    { 35, 9, 5, 0, 7, 0, 1,12},  // 6
    { 44, 9, 6, 0, 8, 0, 1,11},  // 7
    { 53, 5, 7, 0, 9, 0, 1,10},  // 8
    { 58, 0, 8,10, 0, 0, 1, 0},  // 9  front segment GONE NOW
    { 58, 5,11, 0, 9, 0,-1, 8},  // 10
    { 63, 9,12, 0,10, 0,-1, 7},  // 11
    { 72, 9,13, 0,11, 0,-1, 6},  // 12
    { 81, 5,14, 0,12, 0,-1, 5},  // 13
    { 86,12, 4,15, 5,13,-1, 0},  // 14  middle segment
    { 98, 7,16, 0,14, 0,-1, 4},  // 15
    {105, 8,17, 0,15, 0,-1, 3},  // 16
    {113,10,18, 0,16, 0,-1, 2},  // 17
    {123, 5,19, 0,17, 0,-1, 1},  // 18
    {128, 0, 0, 0, 0, 0, 0, 0},  // sentinel
'''

lines = segments.strip().split('\n')
lines = [l.split('//')[0] for l in lines]
lines = [l.replace(',', ' ') for l in lines]
lines = [l.replace('{', ' ') for l in lines]
lines = [l.replace('}', ' ') for l in lines]
lines = [l.split() for l in lines]
lines = [[int(i) for i in l] for l in lines]
segments = [Segment(*l) for l in lines]


def clip(i):
    return 255 if i < 0 or i > 131 else i


class Step:
    def __init__(self, i, pixel_i=-1, seg=None):
        self.i = i
        self.pixel_i = pixel_i
        self.seg = seg

    def __str__(self):
        return '{% 3d, % 3d}, /* seg %s */' % (self.i, self.pixel_i, self.seg)


class Path:
    def __init__(self, name, seg_list, ends=False):
        self.name = name
        self.steps = {}
        for seg in seg_list:
            segment = segments[abs(seg)]
            if seg > 0:
                r = range(segment.length)
            else:
                r = range(segment.length - 1, -1, -1)
            i = 0
            for idx in r:
                self.steps[i] = Step(i=i)
                self.steps[i].pixel_i = segment.offset + idx
                self.steps[i].seg = seg
                i += 1
            if ends:
                for j in range(i, i + 20):
                    self.steps[j] = Step(i=j)
                for j in range(1, -21, -1):
                    self.steps[j] = Step(i=j)

    def __str__(self):
        # PYTHON IS BAD
        s = "step_t path_{name}[] =".format(name=self.name)
        s += " {\n"
        for step in self.steps.values():
            s += str(step)
        s += '\n};\n'
        s += 'uint16_t path_{name}_len = sizeof(path_{name}) / sizeof(path_{name}[0]);'.format(name=self.name)
        return s


path_l = Path('l', [1, 2, 3, 4, -14, 5, 6, 7, 8, 9], ends=True)
path_r = Path('r', [-18, -17, -16, -15, -14, -13, -12, -11, -10, 9], ends=True)
path_b = Path('b', [1, 2, 3, 4, -14, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18], ends=True)

print(path_l)
print(path_r)
print(path_b)
