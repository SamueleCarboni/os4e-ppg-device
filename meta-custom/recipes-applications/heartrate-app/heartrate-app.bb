DESCRIPTION = "Heart Rate Reader application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

RDEPENDS_${PN} = "bash"

SRC_URI = " \
		file://heartrate-app.c \
		file://create_device.sh \
		"

S = "${WORKDIR}"

do_compile() {
	${CC} ${CFLAGS} heartrate-app.c -lm ${LDFLAGS} -o heartrate-app
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 heartrate-app ${D}${bindir}
	install -m 0755 create_device.sh ${D}${bindir}
}
