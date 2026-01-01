from tkinter import *
import ctypes
import pyperclip
from PIL import Image, ImageTk, ImageDraw

user32 = ctypes.windll.user32
user32.SetProcessDPIAware()
[screen_width_init, screen_heigth_init] = [user32.GetSystemMetrics(0), user32.GetSystemMetrics(1)]

# ===== Settings =====
diameter = 15
cut = 3
btn_size = 20
btn_spacing = 2
alpha_normal = 80   # 0–255 transparency for normal
alpha_selected = 255  # fully opaque when clicked

# ===== Variables for image control =====
img_offset_x = 0
img_offset_y = 0
zoom_scale = 1.0

# ===== Functions =====
def on_closing():
    filespace.destroy()

def clear_all():
    for key in button_images:
        canvas.itemconfig(button_images[key], image=img_normal)
        button_states[key] = False
    update_canvas()

def copier():
    copy_list = [key for key, selected in button_states.items() if selected]
    pyperclip.copy(python_to_c(copy_list))

def python_to_c(num_list):
    delimiter = ", "
    joined_string = delimiter.join(str(num) for num in num_list)
    return "const int ROW[] = {" + joined_string + "};"

def toggle_button(bid):
    button_states[bid] = not button_states[bid]
    if button_states[bid]:
        canvas.itemconfig(button_images[bid], image=img_selected)
    else:
        canvas.itemconfig(button_images[bid], image=img_normal)

def make_circle_image(size, color, alpha):
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    draw.ellipse((0, 0, size-1, size-1), fill=(color[0], color[1], color[2], alpha))
    return ImageTk.PhotoImage(img)

def update_canvas():
    filespace.update_idletasks()

def update_background():
    """Redraw background image with pan & zoom."""
    global bg_resized_tk
    # Apply zoom
    new_width = int(grid_width * zoom_scale)
    new_height = int(grid_height * zoom_scale)
    bg_resized = bg_image.resize((new_width, new_height), Image.LANCZOS)
    bg_resized_tk = ImageTk.PhotoImage(bg_resized)
    # Clear and redraw background
    canvas.delete("bg")
    canvas.create_image(img_offset_x, img_offset_y, image=bg_resized_tk, anchor="nw", tags="bg")
    # Redraw buttons on top
    for bid in button_images.values():
        canvas.lift(bid)

def on_pan_x(val):
    global img_offset_x
    img_offset_x = int(val)
    update_background()

def on_pan_y(val):
    global img_offset_y
    img_offset_y = int(val)
    update_background()

def on_zoom(val):
    global zoom_scale
    zoom_scale = float(val)
    update_background()

# ===== GUI =====
filespace = Tk()
filespace.title("LED Finder")

clear_frame = Frame(filespace)
clear_frame.pack()

clear_frame_button = Button(clear_frame, text="Clear All", command=clear_all)
clear_frame_button.pack(side=LEFT, pady=10, padx=10)

copy_frame_button = Button(clear_frame, text="Copy", command=copier)
copy_frame_button.pack(side=RIGHT, pady=10, padx=10)

# Sliders container (below buttons)
slider_frame = Frame(filespace)
slider_frame.pack(pady=5,side=TOP)

pan_x_slider = Scale(slider_frame, from_=-200, to=200, orient=HORIZONTAL, label="Pan X", command=on_pan_x)
pan_x_slider.set(0)
pan_x_slider.pack(side=LEFT, padx=5)

pan_y_slider = Scale(slider_frame, from_=-200, to=200, orient=HORIZONTAL, label="Pan Y", command=on_pan_y)
pan_y_slider.set(0)
pan_y_slider.pack(side=LEFT, padx=5)

zoom_slider = Scale(slider_frame, from_=0.5, to=2.0, resolution=0.05, orient=HORIZONTAL, label="Zoom", command=on_zoom)
zoom_slider.set(1.0)
zoom_slider.pack(side=LEFT, padx=5)

grid_width = diameter * (btn_size + btn_spacing)
grid_height = diameter * (btn_size + btn_spacing)
filespace.geometry(f"{grid_width+40}x{grid_height+160}")

# Original background image
bg_image = Image.open("sans.PNG")
bg_image = bg_image.resize((grid_width, grid_height), Image.LANCZOS)

canvas = Canvas(filespace, width=grid_width, height=grid_height, highlightthickness=0)
canvas.place(x=20, y=120)

# Button images
img_normal = make_circle_image(btn_size, (0, 255, 0), alpha_normal)
img_selected = make_circle_image(btn_size, (0, 255, 0), alpha_selected)
img_special = make_circle_image(btn_size, (0, 255, 255), alpha_selected)  # cyan

buttons_dict = {}
button_images = {}
button_states = {}
counter = 1

# Draw initial background
bg_resized_tk = ImageTk.PhotoImage(bg_image)
canvas.create_image(0, 0, image=bg_resized_tk, anchor="nw", tags="bg")

# Place buttons
for y in range(diameter):
    for x in range(diameter):
        if y < cut-x or y >= diameter - cut+x or y < cut+x+1 - diameter or y >= 2*diameter - cut-x-1:
            continue
        img = img_special if counter == 101 else img_normal
        bid = canvas.create_image(x*(btn_size+btn_spacing), y*(btn_size+btn_spacing),
                                  image=img, anchor="nw")
        button_images[counter] = bid
        button_states[counter] = (counter == 101)
        canvas.tag_bind(bid, "<Button-1>", lambda e, b=counter: toggle_button(b))
        counter += 1



filespace.protocol("WM_DELETE_WINDOW", on_closing)
filespace.mainloop()


