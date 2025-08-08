import time
import keyboard

def clear():
    keyboard.press('ctrl')
    keyboard.press('l')
    keyboard.release('ctrl')
    keyboard.release('l')


def pattern(diameter,cut,inside_margin_out,inner_cut):

    inside_margin = int(diameter/2) -  inside_margin_out+1
    inner_diameter = diameter - 2 * inside_margin
    grid = ''
    for y in range(diameter):
        line = ''
        for x in range(diameter):
            # Check if (x, y) is within the outer clipped square
            if y < cut - x or y >= diameter - cut + x or y < cut + x + 1 - diameter or y >= 2 * diameter - cut - x - 1:
                line += '  '
            else:
                # Coordinates mapped to inner square region
                inner_x = x - inside_margin
                inner_y = y - inside_margin

                if 0 <= inner_x < inner_diameter and 0 <= inner_y < inner_diameter:
                    # Check if (inner_x, inner_y) is inside inner clipped square
                    in_clipped = not (
                        inner_y < inner_cut - inner_x or
                        inner_y >= inner_diameter - inner_cut + inner_x or
                        inner_y < inner_cut + inner_x + 1 - inner_diameter or
                        inner_y >= 2 * inner_diameter - inner_cut - inner_x - 1
                    )
                    if in_clipped:
                        # Now check if it's on the edge (i.e., has any neighboring cell not in clipped area)
                        edge = False
                        for dx, dy in [(-1,0),(1,0),(0,-1),(0,1)]:  # 4-neighbor
                            nx, ny = inner_x + dx, inner_y + dy
                            if not (0 <= nx < inner_diameter and 0 <= ny < inner_diameter):
                                edge = True
                                break
                            ny_cond = not (
                                ny < inner_cut - nx or
                                ny >= inner_diameter - inner_cut + nx or
                                ny < inner_cut + nx + 1 - inner_diameter or
                                ny >= 2 * inner_diameter - inner_cut - nx - 1
                            )
                            if not ny_cond:
                                edge = True
                                break
                        if edge:
                            line += ' o'
                        else:
                            line += '  '
                    else:
                        line += '  '
                else:
                    line += '  '
        grid = grid+line + '\n'
    print(grid)


diameter = 15
cut = 3

inside_margin_out =  8
inner_cut = 3
# margin_array   =  [1,2,3,4,5,6,7,8]
# inner_cut_array = [0,0,1,1,2,2,2,3]

margin_array   =  [1,2,3,4,5,6,7,8,9]
inner_cut_array = [0,0,0,0,0,0,0,0,0]
print('\n'*10)
for i in range(0,len(inner_cut_array)):
    clear()
    pattern(diameter,cut,margin_array[i],inner_cut_array[i])
    time.sleep(0.15)

