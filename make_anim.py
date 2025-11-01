import matplotlib.pyplot as plt
import matplotlib.image as img
import matplotlib.animation as animation

fig, ax = plt.subplots()

flist = [f'./DemoFigs/gen_{i}.png' for i in range(1, 101)]

ims = []
for item in flist:
    imArr = img.imread(item)
    im = ax.imshow(imArr)
    ax.get_xaxis().set_visible(False)
    ax.get_yaxis().set_visible(False)
    ims.append([im])

ani = animation.ArtistAnimation(fig, ims, interval=50, blit=True,
                                repeat_delay=1000)

ani.save('anim1.gif')