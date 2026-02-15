FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    socat \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m ctf

WORKDIR /home/ctf

# Copy challenge files
COPY challenge/heap_haven .
COPY challenge/flag.txt .
COPY challenge/libc.so.6 .

# Set permissions
RUN chown -R root:ctf /home/ctf && \
    chmod 750 /home/ctf && \
    chmod 550 /home/ctf/heap_haven && \
    chmod 440 /home/ctf/flag.txt

EXPOSE 1337

CMD socat TCP-LISTEN:1337,reuseaddr,fork EXEC:./heap_haven,stderr