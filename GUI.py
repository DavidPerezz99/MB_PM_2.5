import gspread
from oauth2client.service_account import ServiceAccountCredentials
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import datetime
from IPython.display import HTML

# Configuración de la autenticación
scope = ["https://spreadsheets.google.com/feeds", "https://www.googleapis.com/auth/drive"]
creds_file = "tribal-spanner-428114-a9-c97585ef299f.json"

# Autenticación con Google Sheets
try:
    creds = ServiceAccountCredentials.from_json_keyfile_name(creds_file, scope)
    client = gspread.authorize(creds)
except Exception as e:
    print("Error al autenticar con Google Sheets:", e)
    exit(1)

# Abre la hoja de cálculo y selecciona la hoja activa
spreadsheet_id = "1U2DHCZ90NcyirM_p22luc3ieco4Ib7S8D-3QVkKBnQ8"
sheet = client.open_by_key(spreadsheet_id).sheet1

def get_data():
    try:
        data = sheet.get_all_records(numericise_ignore=["all"], value_render_option="UNFORMATTED_VALUE")
        df = pd.DataFrame(data)
        df['Date'] = pd.to_datetime(df['Date'])
        return df
    except Exception as e:
        print("Error al obtener datos de Google Sheets:", e)
        return pd.DataFrame(columns=['Date', 'Value'])

# Configuración inicial de la gráfica
fig, ax = plt.subplots()
line, = ax.plot_date([], [], '-')
ax.set_title('Datos en tiempo real')
ax.set_xlabel('Fecha y Hora')
ax.set_ylabel('Valor')
fig.autofmt_xdate()

def update(frame):
    df = get_data()
    if not df.empty:
        ax.clear()
        ax.plot(range(len(df.Value.tolist()[-500:])), df.Value.tolist()[-500:])
        ax.set_title('Datos en tiempo real')
        ax.set_xlabel('Muestra')
        ax.set_ylabel('°Cels')
        ##fig.autofmt_xdate()

    return line,

# Crear la animación y mantener una referencia a ella
ani = FuncAnimation(fig, update, interval=1000, frames = None, cache_frame_data=False)

# Mostrar la gráfica
plt.show()
