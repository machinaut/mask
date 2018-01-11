particles:
- start off-mask and end off-mask
    - extend pixel array by 256 in both directions?  (maybe not if avoidable)
    - special case particle->i <0 or >STRIP_LEN
    - account for pixel array using 255 as sentinel value
- subpixel rendering
    - have a point of peak brightness between pixels
    - interpolate based on velocity & time
    - base glow falloff on the center point
    - maybe single byte as subpixel center
