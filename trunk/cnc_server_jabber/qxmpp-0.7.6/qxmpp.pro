include(qxmpp.pri)
QXMPP_LIBRARY_TYPE=staticlib

TEMPLATE = subdirs

SUBDIRS = src

android {
} else {
    SUBDIRS += doc#tests examples doc
    INSTALLS += htmldocs
}

CONFIG += ordered

# Documentation generation
docs.commands = cd doc/ && $(QMAKE) && $(MAKE) docs

# Source distribution
QXMPP_ARCHIVE = qxmpp-$$QXMPP_VERSION
dist.commands = \
    $(DEL_FILE) -r $$QXMPP_ARCHIVE && \
    $(MKDIR) $$QXMPP_ARCHIVE && \
    git archive master | tar -x -C $$QXMPP_ARCHIVE && \
    $(COPY_DIR) doc/html $$QXMPP_ARCHIVE/doc && \
    tar czf $${QXMPP_ARCHIVE}.tar.gz $$QXMPP_ARCHIVE && \
    $(DEL_FILE) -r $$QXMPP_ARCHIVE
dist.depends = docs

# Install rules
htmldocs.files = doc/html
htmldocs.path = $$PREFIX/share/doc/qxmpp
htmldocs.CONFIG += no_check_exist directory

QMAKE_EXTRA_TARGETS += dist docs
