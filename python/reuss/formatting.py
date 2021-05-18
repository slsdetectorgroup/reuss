class color:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNINGY = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    MAGENTA = '\033[35m'

    @staticmethod
    def ok(s):
        return f'{color.OKGREEN}{s}{color.ENDC}'

    @staticmethod
    def clear(s):
        return f'{color.ENDC}{s}'

    @staticmethod
    def warning(s):
        return f'{color.WARNINGY}WARNING: {s}{color.ENDC}'

    @staticmethod
    def info(s):
        return f'{color.OKBLUE}{s}{color.ENDC}'

    @staticmethod
    def red(s):
        return f'{color.FAIL}{s}{color.ENDC}'

    @staticmethod
    def magenta(s):
        return f'{color.MAGENTA}{s}{color.ENDC}'

    @staticmethod
    def yellow(s):
        return f'{color.WARNINGY}{s}{color.ENDC}'

    @staticmethod
    def error(s):
            return f'{color.FAIL}ERROR: {s}{color.ENDC}'

    @staticmethod
    def plotc(s):
        return f'{color.MAGENTA}{s}{color.ENDC}'