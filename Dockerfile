# Dockerfile for EvaluateSegmentation.
#
# Build image:
#   docker build --tag evaluatesegmentation .
#
# Run image:
#   docker run --rm -it -v $PWD:/data evaluatesegmentation /data/reference.nii.gz /data/segmentation.nii.gz

ARG MAKE_JOBS=1

FROM alpine:3.12.0 AS base
FROM base AS builder

RUN apk add --no-cache \
        cmake \
        g++ \
        gcc \
        git \
        linux-headers \
        make \
        perl

ARG MAKE_JOBS
WORKDIR /opt/itk-src
RUN wget -O- -q https://github.com/InsightSoftwareConsortium/ITK/releases/download/v5.1.1/InsightToolkit-5.1.1.tar.gz \
    | tar xz --strip 1 \
    && mkdir build \
    && cd build \
    && cmake -DCMAKE_INSTALL_PREFIX=/opt/itk -DCMAKE_COLOR_MAKEFILE=OFF .. \
    && make --jobs "$MAKE_JOBS" \
    && make install

WORKDIR /opt/evaluate-segmentation
COPY . .
RUN mkdir build \
    && cd build \
    && cmake -DITK_DIR=/opt/itk/lib/cmake/ITK-5.1 ../source \
    && make --jobs "$MAKE_JOBS"

FROM base
COPY --from=builder /opt/evaluate-segmentation/build/EvaluateSegmentation /usr/local/bin/
RUN apk add --no-cache libgcc libstdc++
WORKDIR /work
ENTRYPOINT ["/usr/local/bin/EvaluateSegmentation"]
CMD ["--help"]
