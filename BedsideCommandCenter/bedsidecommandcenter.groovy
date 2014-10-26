/**
 *  Bedside Command Center
 *
 *  Author: docwisdom
 *  Date: 2014-10-26
 */

metadata {
	definition (name: "Bedside Command Center", author: "brian@docwisdom.com") {
		capability "Alarm"
		capability "Switch"
		capability "Motion Sensor"

		command "beep1"
		command "beep2"
		command "siren1"
		command "siren2"
	}

	simulator {
		// TODO: define status and reply messages here
        status "active": "zone report :: type: 19 value: 0031"
		status "inactive": "zone report :: type: 19 value: 0030"

	}

	tiles {
		standardTile("switch", "device.switch", canChangeIcon: false) {
			state "off", label: '${name}', action: "switch.on", icon: "st.Lighting.light13", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch.off", icon: "st.Lighting.light13", backgroundColor: "#79b821", iconColor: "#ffffff"
		}

		standardTile("motion", "device.motion") {
			state("active", label:'motion', icon:"st.motion.motion.active", backgroundColor:"#53a7c0")
			state("inactive", label:'no motion', icon:"st.motion.motion.inactive", backgroundColor:"#ffffff")
		}

        standardTile("alarm", "device.alarm", canChangeIcon: false) {
			state "off", label:'off', action:'alarm.strobe', icon:"st.alarm.alarm.alarm", backgroundColor:"#ffffff"
			state "strobe", label:'strobe!', action:'alarm.off', icon:"st.alarm.alarm.alarm", backgroundColor:"#e86d13"
			state "siren", label:'siren!', action:'alarm.off', icon:"st.alarm.alarm.alarm", backgroundColor:"#e86d13"
			state "both", label:'alarm!', action:'alarm.off', icon:"st.alarm.alarm.alarm", backgroundColor:"#e86d13"
		}


		main (["switch","alarm","motion"])
		details(["switch","alarm","motion"])
	}
}

def parse(String description) {
	def name = null
	def value = description
	def descriptionText = null
    def message = zigbee.parse(description)?.text
    log.debug "Arduino Sent: $message"


    if(message == "motion.active") {
    	sendEvent(name: "motion", value: "active")
		descriptionText = "${device.displayName} detected motion&&"
    }
    else if(message =="motion.inactive") {
    	sendEvent(name: "motion", value: "inactive")
		descriptionText = "${device.displayName} motion has stopped"
      }
    else if(message =="src.restart") {
        sendEvent(name: "switch", value: "off")
		descriptionText = "${device.displayName} is now online"
        sendEvent(name: "alarm", value: "off")
        sendEvent(name: "water", value: "dry")
		sendEvent(name: "contact", value: "closed")

	}
    else if(message =="return.on") {
        sendEvent(name: "switch", value: "on")
		descriptionText = "${device.displayName} is switched on"
    }

    else if(message =="return.off") {
        sendEvent(name: "switch", value: "off")
		descriptionText = "${device.displayName} is switched off"
        sendEvent(name: "alarm", value: "off")
        sendEvent(name: "color", value: "dark")

    }
    else if(message =="return.siren") {
        sendEvent(name: "alarm", value: "siren")
		descriptionText = "${device.displayName} is in alarm (siren only)"
        sendEvent(name: "switch", value: "on")

    }
    else if(message =="return.strobe") {
        sendEvent(name: "alarm", value: "strobe")
		descriptionText = "${device.displayName} is in alarm (strobe only)"
        sendEvent(name: "switch", value: "on")

    }
    else if(message =="return.both") {
        sendEvent(name: "alarm", value: "both")
		descriptionText = "${device.displayName} is in alarm"
        sendEvent(name: "switch", value: "on")

    }


def result = createEvent(
		name: name,
		value: value,
		descriptionText: descriptionText
	)

	log.debug "Parse returned ${result?.descriptionText}"
	return result
}

def on() {
	log.debug "Executing 'on'"
	zigbee.smartShield(text: "on").format()
}

def off() {
	log.debug "Executing 'off'"
	zigbee.smartShield(text: "off").format()
}



def strobe() {
	log.debug "Executing 'strobe'"
    zigbee.smartShield(text: "strobe").format()
//   sendEvent(name: "alarm", value: "strobe")
}

def siren() {
	log.debug "Executing 'siren'"
    zigbee.smartShield(text: "siren").format()
}

def both() {
	log.debug "Executing 'both'"
    zigbee.smartShield(text: "strobe").format()
}
def alarmoff() {
	log.debug "Executing 'alarmoff'"
    zigbee.smartShield(text: "alarmoff").format()
}


def beep1() {
	log.debug "Executing 'beep1'"
  	zigbee.smartShield(text: "beep1").format()
}
def beep2() {
	log.debug "Executing 'beep2'"
  	zigbee.smartShield(text: "beep2").format()
}


def siren1() {
	log.debug "Executing 'siren1'"
  	zigbee.smartShield(text: "siren1").format()
}
def siren2() {
	log.debug "Executing 'siren2'"
  	zigbee.smartShield(text: "siren2").format()
}
