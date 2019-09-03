import os, sys
from PIL import Image

check_for_drop = False

if len(sys.argv) is 1 and check_for_drop:
	print("You have to drag files to this script instead of running it!")
	input()
	quit()

#img_src = sys.argv[1]
img_src = "./player.png"
rgb_file = open("./rgb.txt", 'w')
alpha_file = open("./alphaMap.txt", 'w')

def rgb_hex565(red, green, blue):
    # take in the red, green and blue values (0-255) as 8 bit values and then combine
    # and shift them to make them a 16 bit hex value in 565 format.
	hex_color = ("0x%0.4X" % ((int(red / 255 * 31) << 11) | (int(green / 255 * 63) << 5) | (int(blue / 255 * 31))))
	return (f"{hex_color}")
	
print(f"Converting file: {img_src}")

image = Image.open(img_src)
pixels = image.load()
rgba_pixels = image.convert('RGBA')

alpha_file.write("{\n")
rgb_file.write("{\n")

for x in range(0, image.size[0]):
	for y in range(0, image.size[1]):
		r,g,b,a = rgba_pixels.getpixel((y, x))
		
		rgb_file.write(f"{rgb_hex565(r, g, b)}")
		
		#Draw the alpha map as binary data
		if a == 0:
			alpha_file.write("true")
		else:
			alpha_file.write("false")
		
		#Write the separator
		if y != image.size[1] - 1 or x != image.size[0] - 1:
			alpha_file.write(",")
			rgb_file.write(",")
			
	alpha_file.write("\n")
	rgb_file.write("\n")
	
alpha_file.write("}")
rgb_file.write("}")

rgb_file.close()
alpha_file.close()

print("Done! Press any key to leave...")
input()