const SerialPort = require('serialport')
const Readline = SerialPort.parsers.Readline
const parser = new Readline()

let path = null
let port = null


function init() {
	process.argv.forEach((val, index) => {
		if (val === '-p' || val === '--port') {
			path = process.argv[index + 1]
			if (typeof path !== 'string') {
				console.log("Couldn't parse port")
				process.exit()
			}
		}
	})

	if (path !== null) {
		port = new SerialPort(path, { baudRate: 19200 })
		port.pipe(parser)
		// parser.on('data', console.log)
		port.on('data', function (data) {
			console.log(data.toString("ascii"));
		});
		port.write('ROBOT PLEASE RESPOND\n')
	} else {
		console.log('no port defined')
		process.exit()
	}
}

init()
