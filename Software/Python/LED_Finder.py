from tkinter import *
from tkinter import filedialog
import ctypes
import pyperclip
import ast

user32 = ctypes.windll.user32
user32.SetProcessDPIAware()
[screen_width_init, screen_heigth_init] = [user32.GetSystemMetrics(0), user32.GetSystemMetrics(1)]



##### Functions

def save_file():
    file_path = filedialog.asksaveasfilename(
        defaultextension=".txt",  # Default extension if none is provided by the user
        filetypes=[
            ("Text files", "*.txt"),
            ("All files", "*.*")
        ]
    )
    
    copy_list = []    
    for key in buttons_dict:
        colorer = buttons_dict[key].cget("bg")
        if colorer == 'green':
            copy_list.append(key)
            
            
    with open(file_path, "w") as file:
        file.write(python_to_c(copy_list))

def open_file():

    file_path = filedialog.askopenfilename(
        title="Select a File",
        initialdir="/",  # Optional: set initial directory
        filetypes=(("Text files", "*.txt"), ("All files", "*.*"))  # Optional: filter file types
    )
    
    with open(file_path, 'r') as file:
        for line in file:
            liner = line.strip()
    liner = liner.replace("const int ROW[] = {", "[")
    liner = liner.replace("};", "]")
    lst = ast.literal_eval(liner)
    
    clear_all()
    for key in buttons_dict:
        if key in lst:
            buttons_dict[key].config(bg='green') 

def on_closing():
    filespace.destroy()
    
def clear_all():
    for key in buttons_dict:
        buttons_dict[key].config(bg='SystemButtonFace')
        if key == 101:
            buttons_dict[key].config(bg='cyan')           

def copier():
    copy_list = []
    for key in buttons_dict:
        colorer = buttons_dict[key].cget("bg")
        if colorer == 'green':
            copy_list.append(key)

    pyperclip.copy(python_to_c(copy_list))

def python_to_c(num_list):
    delimiter = ", "
    joined_string = delimiter.join(str(num) for num in num_list)

    text = "const int ROW[] = {" + joined_string + "};"
    return text

def change_button_color(buttoner):
    colorer = buttons_dict[buttoner].cget("bg")
    
    if colorer == 'SystemButtonFace' or colorer == 'cyan':
        buttons_dict[buttoner].config(bg='green') 
    else:
        buttons_dict[buttoner].config(bg='SystemButtonFace')
        if buttoner == 101:
            buttons_dict[buttoner].config(bg='cyan')        


##### GUI Code
filespace = Tk()
filespace.title('LED Finder')

# get screen size
screen_width = screen_width_init
screen_height = screen_heigth_init/2 + 100
 
#position filespace on the left
filespace.geometry(str(int(screen_width/2)) + 'x' + str(int(screen_height)+50))
filespace.geometry("+{}+{}".format(-10,0))

menubar = Menu(filespace)
filemenu = Menu(menubar, tearoff = 0)
filemenu.add_command(label="Save To File", command = save_file)
filemenu.add_command(label="Open Previous", command = open_file)
menubar.add_cascade(label="File", menu=filemenu)

clear_frame = Frame(filespace)
clear_frame.pack()



clear_frame_button = Button(clear_frame,text="Clear All", command = clear_all)
clear_frame_button.pack(side = LEFT,pady = 15, padx = 10)

copy_frame_button = Button(clear_frame,text="Copy", command = copier)
copy_frame_button.pack(side = RIGHT,pady = 15, padx = 10)

empty_frame = Frame(filespace)
empty_frame.pack(side = TOP,pady = 10)

buttons_dict = {}
frames_dict = {}

pixel = PhotoImage(width=1, height=1)



counter = 1

diameter = 15
cut = 3
for y in range(0,diameter,1):
    frames_dict['Line ' + str(y+1)] = Frame(filespace)
    frames_dict['Line ' + str(y+1)].pack(side = TOP)
    
    for x in range(0,diameter,1):        
        if y < cut-x or y >=diameter - cut+x or y < cut+x+1 - diameter or y >= 2*diameter -cut-x-1:            
            1
        else:
            initial_color = 'SystemButtonFace'
            buttons_dict[counter] = Button(frames_dict['Line ' + str(y+1)], text="", image=pixel, width=20, height=20, compound="center", command=lambda b=counter: change_button_color(b))
            if counter == 101:
                buttons_dict[counter].config(bg='cyan')
            
            buttons_dict[counter].pack(side = LEFT)  
            counter = counter + 1


filespace.config(menu = menubar)
filespace.protocol("WM_DELETE_WINDOW", on_closing)
filespace.mainloop()