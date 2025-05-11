from spyne import Application, rpc, ServiceBase, Unicode
from spyne.protocol.soap import Soap11
from spyne.server.wsgi import WsgiApplication
from datetime import datetime

class TimeService(ServiceBase):

    @rpc(_returns=Unicode)
    def get_datetime(ctx):
        now = datetime.now()
        return now.strftime("%d/%m/%Y %H:%M:%S")

application = Application(
    [TimeService],
    tns='urn:timeservice',
    in_protocol=Soap11(validator='lxml'),
    out_protocol=Soap11()
)

# ---------- Desactivar logs HTTP ----------
from wsgiref.simple_server import make_server, WSGIRequestHandler

class QuietHandler(WSGIRequestHandler):
    def log_message(self, format, *args):
        pass  # Para no imprimir nada

if __name__ == '__main__':
    wsgi_app = WsgiApplication(application)
    server = make_server('127.0.0.1', 8090, wsgi_app, handler_class=QuietHandler)
    server.serve_forever()
