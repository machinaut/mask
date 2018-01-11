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
    { 58, 4, 8,10, 0, 0, 1, 0},  // 9  front segment
    { 62, 5,11, 0, 9, 0,-1, 8},  // 10
    { 67, 9,12, 0,10, 0,-1, 7},  // 11
    { 76, 9,13, 0,11, 0,-1, 6},  // 12
    { 85, 5,14, 0,12, 0,-1, 5},  // 13
    { 90,12, 4,15, 5,13,-1, 0},  // 14  middle segment
    {102, 7,16, 0,14, 0,-1, 4},  // 15
    {109, 8,17, 0,15, 0,-1, 3},  // 16
    {117,10,18, 0,16, 0,-1, 2},  // 17
    {127, 5,19, 0,17, 0,-1, 1},  // 18
    {132, 0, 0, 0, 0, 0, 0, 0},  // sentinel
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


pixels2 = [Pixel(i=i) for i in range(132)]
for seg, segment in enumerate(segments[:-1]):
    # Fill out seg
    for i in range(segment.length):
        pixel = pixels2[segment.offset + i]
        pixel.seg = seg
    # Fill out non-branching next pixels
    for i in range(segment.length - 1):
        if segment.sign > 0:
            pixel = pixels2[segment.offset + i]
            pixel.next_l_i = pixel.next_r_i = segment.offset + i + 1
        else:
            pixel = pixels2[segment.offset + segment.length - 1 - i]
            pixel.next_l_i = pixel.next_r_i = segment.offset + segment.length - 2 - i
    # Fill out non-branching previous pixels
    for i in range(1, segment.length):
        if segment.sign > 0:
            pixel = pixels2[segment.offset + i]
            pixel.prev_l_i = pixel.prev_r_i = segment.offset + i - 1
        else:
            pixel = pixels2[segment.offset + segment.length - 1 - i]
            pixel.prev_l_i = pixel.prev_r_i = segment.offset + segment.length - i
    # Fill out branches
    # import ipdb; ipdb.set_trace()

    if segment.start < len(pixels2):
        start = pixels2[segment.start]
        start.prev_l_i = segments[segment.in0].end
        start.prev_r_i = segments[segment.in1].end
    if segment.end < len(pixels2):
        end = pixels2[segment.end]
        end.next_l_i = segments[segment.out0].start
        end.next_r_i = segments[segment.out1].start


for pixel in pixels2:
    print(pixel)


exit(1)


pixels = []
for seg, segment in enumerate(segments):
    if seg == 0:
        continue  # ignore sentinel
    for idx in range(segment.length):
        i = segment.offset + idx
        next_l_i = next_r_i = i + segment.sign
        next_l_idx = next_r_idx = idx + segment.sign
        next_l_seg = next_r_seg = seg
        if next_l_idx < 0 or next_l_idx >= segment.length:
            if next_l_idx < 0:
                next_l_seg = segment.in0
            elif next_l_idx >= segment.length:
                next_l_seg = segment.out0
            if next_l_seg == 0:
                next_l_idx = -1000
            elif segments[next_l_seg].sign > 0:
                next_l_idx = 0
            else:
                next_l_idx = segments[next_l_seg].length - 1
        if next_r_idx < 0 or next_r_idx >= segment.length:
            if next_r_idx < 0:
                next_r_seg = segment.in1
            elif next_r_idx >= segment.length:
                next_r_seg = segment.out1
            if next_r_seg == 0:
                next_r_idx = -1000
            elif segments[next_r_seg].sign > 0:
                next_r_idx = 0
            else:
                next_r_idx = segments[next_r_seg].length - 1
        prev_l_i = prev_r_i = i - segment.sign
        prev_l_idx = prev_r_idx = idx - segment.sign
        prev_l_seg = prev_r_idx = idx - segment.sign
        if prev_l_idx < 0 or prev_l_idx >= segment.length:
            if prev_l_idx < 0:
                prev_l_seg = segment.out0
            elif prev_l_idx >= segment.length:
                prev_l_seg = segment.in0
            if prev_l_seg == 0:
                prev_l_idx = -1000
            elif segments[prev_l_seg].sign > 0:
                prev_l_idx = segments[prev_l_seg].length - 1
            else:
                prev_l_idx = 0
            prev_l_i = clip(segments[prev_l_seg].offset + prev_l_idx)
        if prev_r_idx < 0 or prev_r_idx >= segment.length:
            if prev_r_idx < 0:
                prev_r_seg = segment.out1
            elif prev_r_idx >= segment.length:
                prev_r_seg = segment.in1
            if prev_r_seg == 0:
                prev_r_idx = -1000
            elif segments[prev_r_seg].sign > 0:
                prev_r_idx = segments[prev_r_seg].length - 1
            else:
                prev_r_idx = 0
            prev_r_i = clip(segments[prev_r_seg].offset + prev_r_idx)
        assert len(pixels) == i
        pixel = Pixel(
            i=i, idx=idx, seg=seg,
            next_l_i=next_l_i, next_l_idx=next_l_idx, next_l_seg=next_l_seg,
            next_r_i=next_r_i, next_r_idx=next_r_idx, next_r_seg=next_r_seg,
            prev_l_i=prev_l_i, prev_l_idx=prev_l_idx, prev_l_seg=prev_l_seg,
            prev_r_i=prev_r_i, prev_r_idx=prev_r_idx, prev_r_seg=prev_r_seg)
        pixels.append(pixel)

for pixel in pixels:
    assert 0 <= pixel.i < 132
    assert 1 <= pixel.seg <= 18
    segment = segments[pixel.seg]
    assert 0 <= pixel.idx < segment.length
    if pixel.next_l_seg == pixel.seg:
        assert pixel.next_l_seg == pixel.next_r_seg == pixel.seg
        assert pixel.next_l_idx == pixel.next_r_idx == pixel.idx + segment.sign

for pixel in pixels:
    print(pixel)
