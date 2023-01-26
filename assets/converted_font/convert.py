# adapted from https://stackoverflow.com/a/67068237/16166019
# renders the given characters to seperate images using a font

from PIL import Image, ImageFont, ImageDraw

font_size = 32 # px
font_color = "#ffffff"
outline_width = 2
outline_color = "#86899f"

# Create Font using PIL
font = ImageFont.truetype("assets/palacemia_titling/Palamecia Titling.otf", font_size)

# available: "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890‘?’“!”(%)[#]{@}/&<-+÷×=>®©$€£¥¢:;,.*"
desired_characters = "abcdefghijklmnopqrstuvwxyz1234567890?!',.:-/_"

# Loop through the characters needed and save to desired location
for character in desired_characters:
    
    # Get text size of character
    width, height = font.getsize(character, stroke_width=outline_width)
    
    # Create PNG Image with that size
    img = Image.new("RGBA", (width, height))
    draw = ImageDraw.Draw(img)
    
    # Draw the character
    draw.text(
        (0, 0), str(character),
        font=font, fill=font_color,
        stroke_width=outline_width, stroke_fill=outline_color
    )
    
    # Save the character as png
    try:
        img.save(f"assets/converted_font/out/char_{ord(character)}.png")
    except:

        print(f"[-] Couldn't Save:\t{character}")
