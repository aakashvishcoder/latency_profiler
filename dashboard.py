import time
import random
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import latency_profiler_py as prof

p = prof.Profiler("order_routing")

fig, ax = plt.subplots()
x_data, y_data = [], []
line, = ax.plot(x_data, y_data, 'r-')
ax.set_title("Real-Time P99 Latency (µs)")
ax.set_xlabel("Time (s)")
ax.set_ylabel("P99 Latency (µs)")

def update(frame):
    for _ in range(10000):
        t0 = p.start()

        _ = sum(range(random.randint(10, 1000))) 
        p.stop(t0)
    

    report = p.report()
    
    x_data.append(frame)
    y_data.append(report.p99_us)
    
    line.set_data(x_data, y_data)
    ax.relim()
    ax.autoscale_view()
    return line,

ani = animation.FuncAnimation(fig, update, interval=1000) 
plt.show()