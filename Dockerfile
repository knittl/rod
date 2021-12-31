FROM gcc:11.2 AS builder

WORKDIR /src
RUN apt-get update && apt-get install xxd

COPY ./ /src/
RUN CCFLAGS=-static make clean build test


FROM alpine:3.15
LABEL maintainer="knittl89+git@googlemail.com" \
      description="rod -- reverse od"

COPY --from=builder /src/rod .
ENTRYPOINT ["./rod"]
