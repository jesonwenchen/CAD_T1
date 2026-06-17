import matplotlib.pyplot as plt
import numpy as np
import matplotlib
matplotlib.use('Agg')

n = [64, 256, 1024, 4096, 16384]
h = [4.91e-02, 1.23e-02, 3.07e-03, 7.67e-04, 1.92e-04]
erro = [4.02e-04, 2.51e-05, 1.57e-06, 9.80e-08, 6.13e-09]

# Log-log plot
plt.figure(figsize=(8, 6))
plt.loglog(h, erro, 'o-', linewidth=2, markersize=8, label='Erro medido')

# Calculate slope (linear fit on log-log data)
log_h = np.log10(h)
log_erro = np.log10(erro)
slope, intercept = np.polyfit(log_h, log_erro, 1)

# Plot theoretical O(h^2)
h_line = np.linspace(min(h), max(h), 100)
# Shift theoretical line slightly for visibility
theo_erro = (h_line**2) * (erro[0]/(h[0]**2))
plt.loglog(h_line, theo_erro, 'r--', label='O(h²)')

plt.grid(True, which="both", ls="--")
plt.xlabel('h (tamanho do passo)')
plt.ylabel('Erro Absoluto |I - 2|')
plt.title(f'Convergência da Regra do Trapézio (Inclinação = {slope:.2f})')
plt.legend()

plt.savefig('convergencia.pdf', bbox_inches='tight')
plt.savefig('convergencia.png', bbox_inches='tight')
print(f'Gráfico gerado com sucesso. Inclinação medida: {slope:.2f}')
