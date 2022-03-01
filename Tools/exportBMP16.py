from PIL import Image
from numpy import asarray
import struct
import sys

pix = (255, 0 ,0)

def rgb2hex(pix):
    r = (pix[0] >> 3) & 0x1F
    g = (pix[1] >> 2) & 0x3F
    b = (pix[2] >> 3) & 0x1F
    return 0xffff - ((r << 11) + (g << 5) + b)



# load the image
image = Image.open(sys.argv[1])
# convert image to numpy array
data = asarray(image)
print(type(data))
# summarize shape
print(data.shape)


f = open(sys.argv[2], 'w')

for pixes in data:
    for pix in pixes:
        pixb = rgb2hex(pix)
        f.write(hex(pixb) + ', ')
    f.write('\n')

f.close()
