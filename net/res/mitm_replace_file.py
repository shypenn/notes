from mitmproxy import ctx
def response(flow):
    ctx.log.info('flow.request.url == ' + str(flow.request.url == 'http://front-demo.dev-ag.56qq.com/index-7ffa4d86fc45646523b6.js'))
    if flow.request.url == 'http://front-demo.dev-ag.56qq.com/index-7ffa4d86fc45646523b6.js':
        fd = open('/Users/roneyrao/temp/index-7ffa4d86fc45646523b6.js', 'rb')
        data = fd.read()
        fd.close()
        flow.response.content = data;
