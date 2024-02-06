#from msilib.schema import tables
from this import d
import pandas as pd
import ast
from datetime import date, datetime
import numpy as np
import os
import influxdb_client
import matplotlib.pyplot as plt
from IPython.display import HTML, Javascript
from influxdb_client import InfluxDBClient, Point, Dialect
from influxdb_client.client.write_api import SYNCHRONOUS

from flask import Flask, redirect, url_for, render_template, request, session, Markup

app = Flask(__name__)
app.secret_key = 'ASDOFJASDFKJA'

# Carrega todos os arquivos da pasta database
def arquivo():
    for _, _, arquivo in os.walk('./database'):
        arquivo
    return arquivo

def dataframe(local):
    if (not os.path.exists(local)):  # Caso não exista o diretório ou o csv
        return True
    df = pd.read_excel(local)
    if df.empty is True: 
        return False
    return df

def influxdb_write_point(ip_c, ip_m):
    bucket = "InteligenciaCondominial"
    org = "igorcosta1410@hotmail.com"
    token = "dBHma1mZugaEQ2EubT2ww0Txbqego5oIednARuMJ8z38CePiMSWPENWh78vN88lbQsHZuSRFYQu3gVWpwYquhg=="
    url="https://us-east-1-1.aws.cloud2.influxdata.com"
    client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)
    write_api = client.write_api(write_options=SYNCHRONOUS)
    query_api = client.query_api()
    _point = Point(ip_c).tag("IP", ip_m).field("Consumo", 0).field("Sistema", 0)
    write_api.write(bucket=bucket, record=[_point])
    client.close()
    return

def influxdb_delete_concentrador(ip_c):
    from influxdb_client import InfluxDBClient, Point, Dialect
    from influxdb_client.client.write_api import SYNCHRONOUS

    bucket = "InteligenciaCondominial"
    org = "igorcosta1410@hotmail.com"
    token = "dBHma1mZugaEQ2EubT2ww0Txbqego5oIednARuMJ8z38CePiMSWPENWh78vN88lbQsHZuSRFYQu3gVWpwYquhg=="
    url="https://us-east-1-1.aws.cloud2.influxdata.com"
    client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)
    delete_api = client.delete_api()
    start = "2022-01-01T00:00:00Z"
    stop = "2023-03-01T00:00:00Z" # Mudar
    delete_api.delete(start, stop, ('_measurement=' + str(ip_c)), bucket=bucket, org=org)
    client.close()
    return 1

def influxdb_delete_ponto(ip_c):
    from influxdb_client import InfluxDBClient, Point, Dialect
    from influxdb_client.client.write_api import SYNCHRONOUS

    bucket = "InteligenciaCondominial"
    org = "igorcosta1410@hotmail.com"
    token = "dBHma1mZugaEQ2EubT2ww0Txbqego5oIednARuMJ8z38CePiMSWPENWh78vN88lbQsHZuSRFYQu3gVWpwYquhg=="
    url="https://us-east-1-1.aws.cloud2.influxdata.com"
    client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)
    delete_api = client.delete_api()
    start = "2022-01-01T00:00:00Z"
    stop = "2023-03-01T00:00:00Z" # Mudar
    delete_api.delete(start, stop, ('IP=' + str(ip_c)), bucket=bucket, org=org)
    client.close()
    return 1 

def read_last_value(ip_c):
    bucket = "InteligenciaCondominial"
    org = "igorcosta1410@hotmail.com"
    token = "dBHma1mZugaEQ2EubT2ww0Txbqego5oIednARuMJ8z38CePiMSWPENWh78vN88lbQsHZuSRFYQu3gVWpwYquhg=="
    url="https://us-east-1-1.aws.cloud2.influxdata.com"
    client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)
    write_api = client.write_api(write_options=SYNCHRONOUS)
    query_api = client.query_api()
    df = query_api.query_data_frame('from(bucket:"InteligenciaCondominial") '
                                    '|> range(start: -30d) '
                                    '|> filter(fn: (r) => r._measurement =="' + ip_c + '") '
                                    '|> last() '
                                    '|> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value") '
                                    '|> keep(columns: ["IP", "Consumo", "Sistema", "_time"])')
    client.close()
    return(df)

def read_value(ip_c, ip_m, i, f):
    bucket = "InteligenciaCondominial"
    org = "igorcosta1410@hotmail.com"
    token = "dBHma1mZugaEQ2EubT2ww0Txbqego5oIednARuMJ8z38CePiMSWPENWh78vN88lbQsHZuSRFYQu3gVWpwYquhg=="
    url="https://us-east-1-1.aws.cloud2.influxdata.com"
    client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)
    write_api = client.write_api(write_options=SYNCHRONOUS)
    query_api = client.query_api()
    df = query_api.query_data_frame('from(bucket:"InteligenciaCondominial") '
                                '|> range(start: '+i+', stop: '+f+') '
                                '|> filter(fn: (r) => r._measurement == "' + ip_c + '") '
                                '|> filter(fn: (r) => r.IP == "' + ip_m + '") '
                                '|> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value") '
                                '|> keep(columns: ["IP", "Consumo", "Sistema", "_time"])')
    client.close()
    return(df)

def consumo_semestral(ip_c, ip_m):
    today = date.today()
    ano=str(today.year)
    mes=str(today.month - 6)
    if today.month - 6 < 1:
        mes=str(today.month+6)
        ano=str(today.year-1)
    bucket = "InteligenciaCondominial"
    org = "igorcosta1410@hotmail.com"
    token = "dBHma1mZugaEQ2EubT2ww0Txbqego5oIednARuMJ8z38CePiMSWPENWh78vN88lbQsHZuSRFYQu3gVWpwYquhg=="
    url="https://us-east-1-1.aws.cloud2.influxdata.com"
    client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)
    write_api = client.write_api(write_options=SYNCHRONOUS)
    query_api = client.query_api()
    df = query_api.query_data_frame('from(bucket:"InteligenciaCondominial") '
                                    '|> range(start: '+ano+'-'+mes.zfill(2)+'-01T00:00:00Z) '
                                    '|> filter(fn: (r) => r._measurement == "' + ip_c + '") '
                                    '|> filter(fn: (r) => r.IP == "' + ip_m + '") '
                                    '|> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value") '
                                    '|> keep(columns: ["Consumo", "_time"])')
    client.close()
    consumo_diario = []
    x = 0
    while x < df.shape[0]:
        df.loc[x, "_time"] = str(df.loc[x, "_time"].strftime('%m'))
        df.loc[x, "Consumo"] = round(ast.literal_eval('0x' + str(df.loc[x, "Consumo"])), 2)
        x+=1
    x = 0
    s = 0
    while x < 6:
        dq = df.copy()
        if (today.month - x) < 1:
            dq.drop(dq.loc[dq['_time'] != str(today.month - x +12).zfill(2)].index, inplace=True)
        else:
            dq.drop(dq.loc[dq['_time'] != str(today.month - x).zfill(2)].index, inplace=True)
        if dq.shape[0] > 0:
            if s == 0:
                ultimo = dq["Consumo"].max()
                s = 1
            consumo_diario.append((ultimo - dq["Consumo"].min())/100)
            ultimo = dq["Consumo"].min()
        else: 
            consumo_diario.append(0)
        x+=1
    consumo_diario.reverse()
    return(consumo_diario)

def meses_semestral():
    today = date.today()
    meses_semestral = []
    labels = [
        'JAN', 'FEV', 'MAR', 'ABR',
        'MAI', 'JUN', 'JUL', 'AGO',
        'SET', 'OUT', 'NOV', 'DEZ'
    ]
    t = 0
    while t < 6:
        meses_semestral.append(labels[int(today.month) - (t+1)])
        t+=1
    meses_semestral.reverse()
    return(meses_semestral)

def consumo_diario(ip_c, ip_m):
    hora= datetime.now()
    bucket = "InteligenciaCondominial"
    org = "igorcosta1410@hotmail.com"
    token = "dBHma1mZugaEQ2EubT2ww0Txbqego5oIednARuMJ8z38CePiMSWPENWh78vN88lbQsHZuSRFYQu3gVWpwYquhg=="
    url="https://us-east-1-1.aws.cloud2.influxdata.com"
    client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)
    write_api = client.write_api(write_options=SYNCHRONOUS)
    query_api = client.query_api()
    df = query_api.query_data_frame('from(bucket:"InteligenciaCondominial") '
                                    '|> range(start: -1d) '
                                    '|> filter(fn: (r) => r._measurement == "' + ip_c + '") '
                                    '|> filter(fn: (r) => r.IP == "' + ip_m + '") '
                                    '|> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value") '
                                    '|> keep(columns: ["Consumo", "_time"])')
    client.close()
    consumo_diario = []
    x = 0
    while x < df.shape[0]:
        df.loc[x, "_time"] = str(df.loc[x, "_time"].strftime('%H'))
        df.loc[x, "Consumo"] = round(ast.literal_eval('0x' + str(df.loc[x, "Consumo"])), 2)
        x+=1
    x = 0
    s = 0
    while x < 24:
        dq = df.copy()
        if (hora.hour+3 - x) < 0:
            dq.drop(dq.loc[dq['_time'] != str(hora.hour+3 - x +24).zfill(2)].index, inplace=True)
        else:
            dq.drop(dq.loc[dq['_time'] != str(hora.hour+3 - x).zfill(2)].index, inplace=True)
        if dq.shape[0] > 0:
            if s == 0:
                ultimo = dq["Consumo"].max()
                s = 1
            consumo_diario.append(round((ultimo - dq["Consumo"].min())/100,2))
            ultimo = dq["Consumo"].min()
        else: 
            consumo_diario.append(0)
        x+=1
    consumo_diario.reverse()
    return(consumo_diario)

def horas_diario():
    hora= datetime.now()
    label = ['00h', '01h', '02h', '03h', '04h', '05h', '06h', '07h', '08h', 
            '09h', '10h', '11h', '12h', '13h', '14h', '15h', '16h', '17h',
            '18h', '19h', '20h', '21h', '22h', '23h']
    horas = []
    t = 0
    while t < 24:
        horas.append(label[hora.hour - t])
        t+=1
    horas.reverse()
    return(horas)

def consumo_mensal(ip_c, ip_m):
    today = date.today()
    ano=str(today.year)
    mes=str(today.month)
    dia=str(today.day)
    bucket = "InteligenciaCondominial"
    org = "igorcosta1410@hotmail.com"
    token = "dBHma1mZugaEQ2EubT2ww0Txbqego5oIednARuMJ8z38CePiMSWPENWh78vN88lbQsHZuSRFYQu3gVWpwYquhg=="
    url="https://us-east-1-1.aws.cloud2.influxdata.com"
    client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)
    write_api = client.write_api(write_options=SYNCHRONOUS)
    query_api = client.query_api()
    df = query_api.query_data_frame('from(bucket:"InteligenciaCondominial") '
                                    '|> range(start: '+ano+'-'+mes.zfill(2)+'-01T00:00:00Z) '
                                    '|> filter(fn: (r) => r._measurement == "' + ip_c + '") '
                                    '|> filter(fn: (r) => r.IP == "' + ip_m + '") '
                                    '|> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value") '
                                    '|> keep(columns: ["Consumo", "_time"])')
    client.close()
    consumo_diario = []

    x = 0
    while x < df.shape[0]:
        df.loc[x, "_time"] = str(df.loc[x, "_time"].strftime('%d/%m/%y'))
        df.loc[x, "Consumo"] = round(ast.literal_eval('0x' + str(df.loc[x, "Consumo"])), 2)
        x+=1
        
    x = 1
    s = 0
    while x <= int(dia):
        dq = df.copy()
        dq.drop(dq.loc[dq['_time'] != str(str(x).zfill(2)+'/'+mes.zfill(2)+'/'+ano[2]+ano[3])].index, inplace=True)
        if dq.shape[0] > 0:
            if s == 0:
                ultimo = dq["Consumo"].min()
                s = 1
            consumo_diario.append(round((dq["Consumo"].max() - ultimo)/100,2))
            ultimo = dq["Consumo"].max()
        else: 
            consumo_diario.append(0)
        x+=1
    return(consumo_diario)

def mensal():
    today = date.today()
    dias = []
    t = 0
    while t < int(today.day):
        dias.append(str(t+1)+'ºd')
        t+=1
    return(dias)
    

@app.route("/")
def homepage():
    session['senha'] = None
    session['condominio'] = None
    return render_template("index.html")

@app.route("/empresa")
def empresa():
    session['senha'] = None
    session['condominio'] = None
    return render_template("empresa.html")

@app.route("/tecnologia")
def tecnologia():
    session['senha'] = None
    session['condominio'] = None
    return render_template("tecnologia.html")

@app.route("/produtos")
def produtos():
    session['senha'] = None
    session['condominio'] = None
    return render_template("produtos.html")

@app.route("/projetos")
def projetos():
    session['senha'] = None
    session['condominio'] = None
    return render_template("projetos.html")

@app.route("/login", methods=["POST", "GET"])
def login():
    arq = arquivo()
    arq = sorted(arq) # Ordem alfabética
    c = 0
    while c < len(arq): # Retira o .xlsm do nome do arquivo
        arq[c] = arq[c][:-5]
        c += 1

    if request.method == "POST":
        session["senha"] = request.form["senha"]
        session['condominio'] = request.form["condominios"]
        return redirect(url_for('user'))
    
    return render_template("login.html", arquivos=arq)

@app.route("/login/invalid", methods=["POST", "GET"])
def login2():
    error = "Senha Inválida"  
    arq = arquivo()
    arq = sorted(arq) # Ordem alfabética
    c = 0
    while c < len(arq): # Retira o .xlsm do nome do arquivo
        arq[c] = arq[c][:-5]
        c += 1

    if request.method == "POST":
        session['senha'] = request.form["senha"]
        session['condominio'] = request.form["condominios"]
        return redirect(url_for("user"))
    
    return render_template("login.html", arquivos=arq, error=error)

@app.route("/usuario", methods=["POST", "GET"])
def user():
    if not session.get("senha"):
        redirect(url_for("homepage"))

    arq = arquivo()
    arq = sorted(arq) # Ordem alfabética
    df = dataframe(f"./database/{arq[int(session['condominio'])]}")

    print(session['senha'])

    if (str(session['senha']) == "admin"):
        return redirect(url_for("admin"))

    if (str(session['senha']) != str(df.loc[0, 'Senha'])):
        return redirect(url_for("login2"))
    
    session['condominio'] = str(arq[int(session['condominio'])][:-5])
        
    return redirect(url_for("home"))

@app.route("/usuario/home")
def home():
    if not session.get("condominio"):
        redirect(url_for("homepage"))

    dft = dataframe(f"./database/{session['condominio']}.xlsx")

    # Coloca na lista todos os concentradores do condomínio
    lista = []
    a = 1
    while a < dft.shape[0]:
        i = 0
        b = 0
        while b < len(lista) and i == 0:
            if (str(lista[b]) == str(dft.loc[a, 'IP do Concentrador'])):
                i = 1
            b += 1
        if i == 0:
            lista.append(dft.loc[a, 'IP do Concentrador'])
        a+=1

    # Criar uma tabela com os valores das medições
    dt = pd.DataFrame(columns = ['Nº Apartamento', 'IP Concentrador', 'IP Medidor', 'Tipo do Medidor', 'Consumo [m^3]', 'Sistema', 'Última Medição', 'Data'])
    a = 0
    z = 0
    inativo = 0
    while a < len(lista):
        df = read_last_value(lista[a])
        x = 0
        while x < len(df):
            y = 0
            if type(df) is list:
                while y < df[x].shape[0] and y < dft.shape[0]-1:
                    dt.loc[z, 'IP Concentrador'] = lista[a]
                    dt.loc[z, 'IP Medidor'] = df[x].loc[y, 'IP']
                    #if type(df[x].loc[y, 'Consumo']) is int: 
                    dt.loc[z, 'Consumo [m^3]'] = ast.literal_eval('0x' + str(df[x].loc[y, 'Consumo']))/100
                    if (int(df[x].loc[y, 'Sistema']) == 1): dt.loc[z, 'Sistema'] = "Ativo"
                    else: 
                        dt.loc[z, 'Sistema'] = "Inativo"
                        inativo += 1
                    h = int(df[x].loc[y, '_time'].strftime('%H'))-3
                    d = df[x].loc[y, '_time'].strftime('%d')
                    if h < 0:
                        h = int(int(df[x].loc[y, '_time'].strftime('%H')))+21
                        d = int(df[x].loc[y, '_time'].strftime('%d'))-1
                    dt.loc[z, 'Última Medição'] = df[x].loc[y, '_time'].strftime(str(h).zfill(2)+'h %Mm %Ss')
                    dt.loc[z, 'Data'] = df[x].loc[y, '_time'].strftime(str(d).zfill(2)+'/%m/%y')
                    dt.loc[z, 'Nº Apartamento'] = dft.loc[((dft.index[dft['IP da Telemetria'] == df[x].loc[y, 'IP']].tolist())[0]), 'Número do APT']
                    dt.loc[z, 'Tipo do Medidor'] = dft.loc[((dft.index[dft['IP da Telemetria'] == df[x].loc[y, 'IP']].tolist())[0]), 'Tipo de medidor']
                    y+=1
                    z+=1
            else:
                while y < df.shape[0] and y < dft.shape[0]-1:
                    dt.loc[z, 'IP Concentrador'] = lista[a]
                    dt.loc[z, 'IP Medidor'] = df.loc[y, 'IP']
                    #if type(df.loc[y, 'Consumo']) is int: 
                    dt.loc[z, 'Consumo [m^3]'] = ast.literal_eval('0x' + str(df.loc[y, 'Consumo']))/100
                    if (int(df.loc[y, 'Sistema']) == 1): dt.loc[z, 'Sistema'] = "Ativo"
                    else: 
                        dt.loc[z, 'Sistema'] = "Inativo"
                        inativo += 1
                    h = int(df.loc[y, '_time'].strftime('%H'))-3
                    d = df.loc[y, '_time'].strftime('%d')
                    if h < 0:
                        h = int(int(df.loc[y, '_time'].strftime('%H')))+21
                        d = int(df.loc[y, '_time'].strftime('%d'))-1
                    dt.loc[z, 'Última Medição'] = df.loc[y, '_time'].strftime(str(h).zfill(2)+'h %Mm %Ss')
                    dt.loc[z, 'Data'] = df.loc[y, '_time'].strftime(str(d).zfill(2)+'/%m/%y')
                    print("a")
                    dt.loc[z, 'Nº Apartamento'] = dft.loc[((dft.index[dft['IP da Telemetria'] == df.loc[y, 'IP']].tolist())[0]), 'Número do APT']
                    dt.loc[z, 'Tipo do Medidor'] = dft.loc[((dft.index[dft['IP da Telemetria'] == df.loc[y, 'IP']].tolist())[0]), 'Tipo de medidor']
                    y+=1
                    z+=1
            x+=1
        a+=1

        date = dft.loc[0, 'Número do APT'][8:10] + '/' + dft.loc[0, 'Número do APT'][5:7] + '/' + dft.loc[0, 'Número do APT'][0:4]
        u_data = dt.sort_values(by=['Data'], ascending=False, ignore_index=True).loc[0, 'Data']
        u_hora = dt.sort_values(by=['Última Medição'], ascending=False, ignore_index=True).loc[0, 'Última Medição']

    return render_template("home.html", cond=session['condominio'], endereco=dft.loc[0, 'IP do Concentrador'], 
                            ip_c=len(lista), status="Ativo", instalacao = date,
                            qtd_apt=dft.loc[0, 'Proprietário'], qtd_pontos=(dft.shape[0]-1),
                            tables=[dt.to_html(classes='data', header="true")], inativos=inativo, u_data=u_data, u_hora=u_hora)

@app.route("/usuario/cadastros", methods=["POST", "GET"])
def cadastros():
    if not session.get("condominio"):
        redirect(url_for("homepage"))

    dft = dataframe(f"./database/{session['condominio']}.xlsx")
    x = 1
    lista = list()
    while x < dft.shape[0]:
        lista.append('Apartamento: ' + str(dft.loc[x, 'Número do APT']) + ', Tipo: ' + str(dft.loc[x, 'Tipo de medidor']) + ', IP: ' + str(dft.loc[x, 'IP da Telemetria'])) 
        x += 1

    if request.method == "POST":
        ponto = request.form["ponto"]
        i = request.form["i"]
        f = request.form["f"]
        ip_c = dft.loc[(int(ponto) + 1), 'IP do Concentrador']
        ip_m = dft.loc[(int(ponto) + 1), 'IP da Telemetria']
        df = read_value(ip_c, ip_m, i, f)
        dt = pd.DataFrame(columns = ['Consumo [Pulso]', 'Hodômetro [m^3]','Sistema', 'Horário', 'Data'])
        x = 0
        while x < df.shape[0]:
            dt.loc[x, 'Consumo [Pulso]'] = ast.literal_eval('0x' + str(df.loc[x, 'Consumo']))
            dt.loc[x, 'Hodômetro [m^3]'] = float(int(ast.literal_eval('0x' + str(df.loc[x, 'Consumo'])))*10 + int(dft.loc[(int(ponto) + 1), 'Consumo Inicial']))/1000
            if (int(df.loc[x, 'Sistema']) == 1): dt.loc[x, 'Sistema'] = "Ativo"
            else: dt.loc[x, 'Sistema'] = "Inativo"
            dt.loc[x, 'Horário'] = df.loc[x, '_time'].strftime('%Hh %Mm %Ss')
            dt.loc[x, 'Data'] = df.loc[x, '_time'].strftime('%d/%m/%y')
            x+=1
        dt = dt.sort_values(['Data', 'Horário'], ascending=[False, False], ignore_index=True)

        return render_template("cadastros.html", cond=session['condominio'], list=lista, tables=[dt.to_html(classes='data', header="true")])

    return render_template("cadastros.html", cond=session['condominio'], list=lista)

@app.route("/usuario/graficos", methods=["POST", "GET"])
def graficos():
    if not session.get("condominio"):
        redirect(url_for("homepage"))
    dft = dataframe(f"./database/{session['condominio']}.xlsx")
    x = 1
    lista = list()
    while x < dft.shape[0]:
        lista.append('Apartamento: ' + str(dft.loc[x, 'Número do APT']) + ', Tipo: ' + str(dft.loc[x, 'Tipo de medidor']) + ', IP: ' + str(dft.loc[x, 'IP da Telemetria'])) 
        x += 1

    if request.method == "POST":
        ponto = request.form["ponto"]
        ip_c = dft.loc[(int(ponto) + 1), 'IP do Concentrador']
        ip_m = dft.loc[(int(ponto) + 1), 'IP da Telemetria']
        bar_labels1=meses_semestral()
        bar_values1=consumo_semestral(ip_c, ip_m)
        bar_labels2=horas_diario()
        bar_values2=consumo_diario(ip_c, ip_m)
        bar_labels3=mensal()
        bar_values3=consumo_mensal(ip_c, ip_m)
        x = 0
        gastos = 0
        while x < len(bar_values3):
            gastos += bar_values3[x]
            x+=1
        return render_template("graficos.html", cond=session['condominio'], list=lista, x=1, title1='Consumo Semestral em m^3', max1=max(bar_values1)+1, labels1=bar_labels1, values1=bar_values1, title2='Consumo nas últimas 24 horas em m^3', max2=max(bar_values2)+0.3, labels2=bar_labels2, values2=bar_values2, title3='Consumo neste mês em m^3', max3=max(bar_values3)+0.5, labels3=bar_labels3, values3=bar_values3, gastos=round(gastos*7.71,2))
    
    return render_template("graficos.html", cond=session['condominio'], list=lista, x=0)

@app.route("/usuario/relatorios")
def relatorios():
    if not session.get("condominio"):
        redirect(url_for("homepage"))
    return render_template("relatorios.html", cond=session['condominio'])

@app.route("/usuario/admin")
def admin():
    if str(session['senha']) != 'admin':
        return redirect(url_for("homepage"))
    return render_template("admin.html")

@app.route("/usuario/admin/adicionar_condominio", methods=["POST", "GET"])
def adicionar_condominio():
    if str(session['senha']) != 'admin':
        return redirect(url_for("homepage"))

    if request.method == "POST":
        nome = request.form["nome"]
        endereco = request.form["endereco"]
        apartamentos = request.form["apartamentos"]
        instalacao = request.form["instalacao"]
        key= request.form["key"]

        dft = pd.DataFrame(columns = ['Prédio', 'IP do Concentrador', 'Proprietário', 'Número do APT', 
                                      'Tipo de medidor', 'IP da Telemetria', 'Consumo Inicial', 'Data', 'Senha'])

        dft = dft.append({'Prédio':nome, 'IP do Concentrador':endereco, 'Proprietário':apartamentos, 
                          'Número do APT':instalacao, 'Tipo de medidor':None, 'IP da Telemetria':None, 
                          'Consumo Inicial':None, 'Data':None, 'Senha':key}, ignore_index=True)
        
        dft.to_excel(f"./database/{nome}.xlsx", index = False)

        return redirect(url_for("admin"))

    return render_template("adicionar_condominio.html")

@app.route("/usuario/admin/adicionar_ponto", methods=["POST", "GET"])
def adicionar_ponto():
    if str(session['senha']) != 'admin':
        return redirect(url_for("homepage"))
    
    arq = arquivo()
    arq = sorted(arq) # Ordem alfabética
    c = 0
    while c < len(arq): # Retira o .xlsm do nome do arquivo
        arq[c] = arq[c][:-5]
        c += 1

    if request.method == "POST":
        condominio = request.form["condominio"]
        ip_c = request.form["ip_c"]
        nome = request.form["nome"]
        numero = request.form["numero"]
        data = request.form["data"]
        ip_m = request.form["ip_m"]
        tipo = request.form["tipo"]
        consumo = request.form["consumo"]
        key = request.form["key"]

        influxdb_write_point(ip_c, ip_m)

        arq = arquivo()
        arq = sorted(arq) # Ordem alfabética
        dft = dataframe(f"./database/{arq[int(condominio)]}")

        dft = dft.append({'Prédio':arq[int(condominio)][:-5], 'IP do Concentrador':ip_c, 'Proprietário':nome, 
                          'Número do APT':numero, 'Tipo de medidor':tipo, 'IP da Telemetria':ip_m, 
                          'Consumo Inicial':consumo, 'Data':data, 'Senha':key}, ignore_index=True)

        dft.to_excel(f"./database/{arq[int(condominio)]}", index = False)

        return redirect(url_for("admin"))

    return render_template("adicionar_ponto.html", arquivos=arq)

@app.route("/usuario/admin/visualizar_informacoes", methods=["POST", "GET"])
def visualizar_informacoes():
    if str(session['senha']) != 'admin':
        return redirect(url_for("homepage"))
    arq = arquivo()
    arq = sorted(arq) # Ordem alfabética
    c = 0
    while c < len(arq): # Retira o .xlsm do nome do arquivo
        arq[c] = arq[c][:-5]
        c += 1

    if request.method == "POST":
        session['condominio'] = request.form["condominios"]
        return redirect(url_for("visualizar_informacoes2"))
    return render_template("visualizar_informacoes.html", arquivos=arq)

@app.route("/usuario/admin/visualizar_informacoes2")
def visualizar_informacoes2():
    if str(session['senha']) != 'admin':
        return redirect(url_for("homepage"))
    
    arq = arquivo()
    arq = sorted(arq) 
    dft = dataframe(f"./database/{arq[int(session['condominio'])]}")
    
    return render_template("visualizar_informacoes2.html", tables=[dft.to_html(classes='data', header="true")])


@app.route("/usuario/admin/atualizar")
def atualizar():
    if str(session['senha']) != 'admin':
        return redirect(url_for("homepage"))
    return render_template("atualizar.html")

@app.route("/usuario/admin/deletar_ponto", methods=["POST", "GET"])
def deletar_ponto():
    if str(session['senha']) != 'admin':
        return redirect(url_for("homepage"))

    arq = arquivo()
    arq = sorted(arq) # Ordem alfabética
    c = 0
    while c < len(arq): # Retira o .xlsm do nome do arquivo
        arq[c] = arq[c][:-5]
        c += 1

    if request.method == "POST":
        session['condominio'] = request.form["condominios"]
        return redirect(url_for("deletar_ponto2"))

    return render_template("deletar_ponto.html", arquivos=arq)

@app.route("/usuario/admin/deletar_ponto2", methods=["POST", "GET"])
def deletar_ponto2():
    if str(session['senha']) != 'admin':
        return redirect(url_for("homepage"))

    arq = arquivo()
    arq = sorted(arq) 
    dft = dataframe(f"./database/{arq[int(session['condominio'])]}")
    x = 1
    lista = list()
    while x < dft.shape[0]:
        lista.append('Apartamento: ' + str(dft.loc[x, 'Número do APT']) + ', Tipo: ' + str(dft.loc[x, 'Tipo de medidor']) + ', IP: ' + str(dft.loc[x, 'IP da Telemetria'])) 
        x += 1

    if request.method == "POST":
        ponto = request.form["ponto"]
        s = request.form["s"]
        if (s == 'Sim'):
            influxdb_delete_ponto(dft.loc[(int(ponto) + 1), 'IP da Telemetria'])
            dft = dft.drop(int(ponto) + 1)
            dft.to_excel(f"./database/{arq[int(session['condominio'])]}", index = False)

        return redirect(url_for("admin"))

    return render_template("deletar_ponto2.html", list=lista)

@app.route("/usuario/admin/deletar_condominio", methods=["POST", "GET"])
def deletar_condominio():
    if str(session['senha']) != 'admin':
        return redirect(url_for("homepage"))
    
    arq = arquivo()
    arq = sorted(arq) 
    c = 0
    while c < len(arq): 
        arq[c] = arq[c][:-5]
        c += 1

    if request.method == "POST":
        condominio = request.form["condominios"]
        s = request.form["s"]
        if (s == 'Sim'):
            arq = arquivo()
            arq = sorted(arq)
        
            dft = dataframe(f"./database/{arq[int(condominio)]}")
            lista = list()
            a = 1
            while a < dft.shape[0]:
                i = 0
                b = 0
                while b < len(lista):
                    if lista[b] == dft.loc[a, 'IP do Concentrador']:
                        i = 1
                    b += 1
                if i == 0:
                    lista.append(dft.loc[a, 'IP do Concentrador'])
                a+=1
            a = 0
            while a < len(lista):
                influxdb_delete_concentrador(lista[a])
                a += 1
            os.remove(f"./database/{arq[int(condominio)]}")
        
        return redirect(url_for("admin"))

    return render_template("deletar_condominio.html", arquivos=arq)

if __name__ == "__main__":
    app.run(debug=True)

#haha
# heroku login
# git add .
# git commit -am "make it better"
# git push heroku master