# Copyright 2019 Intel Corporation
# Copyright IBM Corp. All Rights Reserved.
#
# SPDX-License-Identifier: Apache-2.0

TOP = ../../..
include $(TOP)/build.mk

GO_CMDS= cli

build: ${GO_CMDS}

${GO_CMDS}: ${GO_CMDS:=.go}
	$(GO) build $(GOTAGS) $@.go

test: build
	./scenario-run.sh --bootstrap --non-interactive ../../scenario/script

clean: 
	$(GO) clean
