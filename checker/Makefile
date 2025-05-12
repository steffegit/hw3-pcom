# Python + Flask makefile

VENV = .venv
VENV_PYTHON3 = $(VENV)/bin/python3

ADMIN ?= stefan.gatej:930403745117
PROGRAM ?= ../client

all: venv deps

venv: $(VENV_PYTHON3)
$(VENV_PYTHON3):
	python3 -m venv "$(VENV)"

deps: venv
	$(VENV_PYTHON3) -m pip install -r requirements.txt

A ?= --debug --admin "$(ADMIN)"
run:
	$(VENV_PYTHON3) checker.py $(PROGRAM) $(A)

