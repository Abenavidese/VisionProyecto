import psutil
import tkinter as tk
from tkinter import font
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import time

class SystemMonitorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Monitor de Sistema")
        self.root.geometry("800x600")  # Aumentamos el tamaño de la ventana
        
        # Cambiar el fondo de la ventana
        self.root.config(bg="#F0F0F0")

        # Crear un marco (frame) para organizar mejor los elementos
        frame = tk.Frame(self.root, bg="#F0F0F0")
        frame.pack(pady=30, padx=20, fill="both", expand=True)

        # Etiquetas para mostrar información
        self.label_ram = tk.Label(frame, text="Uso de RAM: Cargando...", font=("Arial", 14), bg="#F0F0F0")
        self.label_ram.grid(row=0, column=0, pady=10, padx=10, sticky="w")
        
        self.label_cpu = tk.Label(frame, text="Uso de CPU: Cargando...", font=("Arial", 14), bg="#F0F0F0")
        self.label_cpu.grid(row=1, column=0, pady=10, padx=10, sticky="w")
        
        self.label_cores = tk.Label(frame, text="Núcleos: Cargando...", font=("Arial", 14), bg="#F0F0F0")
        self.label_cores.grid(row=2, column=0, pady=10, padx=10, sticky="w")

        # Inicializar listas para almacenar los datos
        self.ram_usage_list = []
        self.cpu_usage_list = []
        self.time_list = []

        # Crear el gráfico de líneas inicial
        self.fig, self.ax = plt.subplots(figsize=(8, 5))  # Aumentamos el tamaño del gráfico
        self.ax.set_xlabel("Tiempo (s)", fontsize=12)
        self.ax.set_ylabel("Uso (%)", fontsize=12)
        self.ax.set_title("Uso de RAM y CPU en el tiempo", fontsize=14)

        # Mostrar el gráfico en la ventana
        self.canvas = FigureCanvasTkAgg(self.fig, master=frame)
        self.canvas.get_tk_widget().grid(row=3, column=0, pady=20, padx=10)

        # Actualizar la información y el gráfico cada segundo
        self.update_info()

    def update_info(self):
        # Obtener uso de la RAM
        ram = psutil.virtual_memory()
        ram_usage = ram.percent
        
        # Obtener uso de la CPU
        cpu_usage = psutil.cpu_percent(interval=1)
        
        # Obtener el número de núcleos
        cpu_cores = psutil.cpu_count(logical=False)
        
        # Actualizar las etiquetas
        self.label_ram.config(text=f"Uso de RAM: {ram_usage}%")
        self.label_cpu.config(text=f"Uso de CPU: {cpu_usage}%")
        self.label_cores.config(text=f"Núcleos físicos: {cpu_cores}")
        
        # Agregar los datos a las listas
        self.ram_usage_list.append(ram_usage)
        self.cpu_usage_list.append(cpu_usage)
        self.time_list.append(time.time())  # Usamos el tiempo en segundos como eje X
        
        # Limitar el tamaño de las listas (evitar que crezcan indefinidamente)
        if len(self.time_list) > 60:  # Mantener solo los últimos 60 segundos de datos
            self.time_list.pop(0)
            self.ram_usage_list.pop(0)
            self.cpu_usage_list.pop(0)
        
        # Limpiar el gráfico y dibujar las nuevas líneas
        self.ax.clear()
        self.ax.plot(self.time_list, self.ram_usage_list, color="blue", label="Uso de RAM")
        self.ax.plot(self.time_list, self.cpu_usage_list, color="green", label="Uso de CPU")
        
        # Configurar etiquetas y leyenda
        self.ax.set_xlabel("Tiempo (s)", fontsize=12)
        self.ax.set_ylabel("Uso (%)", fontsize=12)
        self.ax.set_title("Uso de RAM y CPU en el tiempo", fontsize=14)
        self.ax.legend()
        
        # Actualizar el gráfico
        self.canvas.draw()

        # Volver a llamar a la función para actualizar la información y el gráfico cada segundo
        self.root.after(1000, self.update_info)

# Crear la ventana principal
root = tk.Tk()
app = SystemMonitorApp(root)

# Iniciar la aplicación
root.mainloop()
