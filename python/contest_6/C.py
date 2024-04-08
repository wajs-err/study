import traceback


def force_load(module):
    with open(module + '.py') as f:
        s = f.read().split('\n')
        exit = False
        while not exit:
            ldict = {}
            try:
                exec('\n'.join(s), globals(), ldict)
                exit = True
            except:
                errors = traceback.format_exc().split('File')
                line = int(errors[-1].split('line ')[-1].split(',')[0].split('\n')[0])
                del s[line - 1]
        return ldict
