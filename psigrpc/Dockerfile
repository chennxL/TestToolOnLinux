FROM ubuntu:24.04

RUN set -x \
    && apt-get update \
    && apt-get install --no-install-recommends --no-install-suggests -y \
        libgrpc++1.51t64 \
        libgomp1 \
    && rm -rf /var/lib/apt/lists/*

ADD psisrv.tar /opt

EXPOSE 50051

STOPSIGNAL SIGQUIT

CMD ["/opt/bin/psisrv"]
