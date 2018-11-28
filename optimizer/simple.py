import math
import sys
import csv
# limitations

# mem, cpu, nssd, per_ssd_iops, tp, pps
prices = {}
with open('instancePrice.csv', newline='') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=',', quotechar='"')
    for i, row in enumerate(spamreader):
        if i == 0:
            continue
        name, price = row[4], float(row[5])
        prices[name] = price

ecss = """ecs.g5.large 2 8 1 30
ecs.g5.xlarge 4 16 1.5 50
ecs.g5.2xlarge 8 32 2.5 80
ecs.g5.3xlarge 12 48 4 90
ecs.g5.4xlarge 16 64 5 100
ecs.g5.6xlarge 24 96 7.5 150
ecs.g5.8xlarge 32 128 10 200
ecs.g5.16xlarge 64 256 20 400
ecs.sn2ne.large 2 8 1 30
ecs.sn2ne.xlarge 4 16 1.5 50
ecs.sn2ne.2xlarge 8 32 2 100
ecs.sn2ne.3xlarge 12 48 2.5 130
ecs.sn2ne.4xlarge 16 64 3 160
ecs.sn2ne.6xlarge 24 96 4.5 200
ecs.sn2ne.8xlarge 32 128 6 250
ecs.sn2ne.14xlarge 56 224 10 450
ecs.ic5.large 2 2 1 30
ecs.ic5.xlarge 4 4 1.5 50
ecs.ic5.2xlarge 8 8 2.5 80
ecs.ic5.3xlarge 12 12 4 90
ecs.ic5.4xlarge 16 16 5 100
ecs.c5.large 2 4 1 30
ecs.c5.xlarge 4 8 1.5 50
ecs.c5.2xlarge 8 16 2.5 80
ecs.c5.3xlarge 12 24 4 90
ecs.c5.4xlarge 16 32 5 100
ecs.c5.6xlarge 24 48 7.5 150
ecs.c5.8xlarge 32 64 10 200
ecs.c5.16xlarge 64 128 20 400
ecs.sn1ne.large 2 4 1 30
ecs.sn1ne.xlarge 4 8 1.5 50
ecs.sn1ne.2xlarge 8 16 2 100
ecs.sn1ne.3xlarge 12 24 2.5 130
ecs.sn1ne.4xlarge 16 32 3 160
ecs.sn1ne.6xlarge 24 48 4.5 200
ecs.sn1ne.8xlarge 32 64 6 250
ecs.r5.large 2 16 1 30
ecs.r5.xlarge 4 32 1.5 50
ecs.r5.2xlarge 8 64 2.5 80
ecs.r5.3xlarge 12 96 4 90
ecs.r5.4xlarge 16 128 5 100
ecs.r5.6xlarge 24 192 7.5 150
ecs.r5.8xlarge 32 256 10 200
ecs.r5.16xlarge 64 512 20 400
ecs.re4.20xlarge 80 960 15 200
ecs.re4.40xlarge 160 1920 30 450
ecs.se1ne.large 2 16 1 30
ecs.se1ne.xlarge 4 32 1.5 50
ecs.se1ne.2xlarge 8 64 2 100
ecs.se1ne.3xlarge 12 96 2.5 130
ecs.se1ne.4xlarge 16 128 3 160
ecs.se1ne.6xlarge 24 192 4.5 200
ecs.se1ne.8xlarge 32 256 6 250
ecs.se1ne.14xlarge 56 480 10 450
ecs.se1.large 2 16 0.5 10
ecs.se1.xlarge 4 32 0.8 20
ecs.se1.2xlarge 8 64 1.5 40
ecs.se1.4xlarge 16 128 3 50
ecs.se1.8xlarge 32 256 6 80
ecs.se1.14xlarge 56 480 10 120
ecs.d1ne.2xlarge 8 32 6 100
ecs.d1ne.4xlarge 16 64 12 160
ecs.d1ne.6xlarge 24 96 16 200
ecs.d1ne-c8d3.8xlarge 32 128 20 200
ecs.d1ne.8xlarge 32 128 20 250
ecs.d1ne-c14d3.14xlarge 56 160 35 450
ecs.d1ne.14xlarge 56 224 35 450
ecs.d1.2xlarge 8 32 3 30
ecs.d1.3xlarge 12 48 4 40
ecs.d1.4xlarge 16 64 6 60
ecs.d1.6xlarge 24 96 8 80
ecs.d1-c8d3.8xlarge 32 128 10 100
ecs.d1.8xlarge 32 128 10 100
ecs.d1-c14d3.14xlarge 56 160 17 180
ecs.d1.14xlarge 56 224 17 180
ecs.i2.xlarge 4 32 1 50
ecs.i2.2xlarge 8 64 2 100
ecs.i2.4xlarge 16 128 3 150
ecs.i2.8xlarge 32 256 6 200
ecs.i2.16xlarge 64 512 10 400
ecs.i2g.2xlarge 8 32 2 100
ecs.i2g.4xlarge 16 64 3 150
ecs.i2g.8xlarge 32 128 6 200
ecs.i2g.16xlarge 64 256 10 400
ecs.i1.xlarge 4 16 0.8 20
ecs.i1.2xlarge 8 32 1.5 40
ecs.i1.3xlarge 12 48 2 40
ecs.i1.4xlarge 16 64 3 50
ecs.i1-c5d1.4xlarge 16 64 3 40
ecs.i1.6xlarge 24 96 4.5 60
ecs.i1.8xlarge 32 128 6 80
ecs.i1-c10d1.8xlarge 32 128 6 80
ecs.i1.14xlarge 56 224 10 120
ecs.hfc5.large 2 4 1 30
ecs.hfc5.xlarge 4 8 1.5 50
ecs.hfc5.2xlarge 8 16 2 100
ecs.hfc5.3xlarge 12 24 2.5 130
ecs.hfc5.4xlarge 16 32 3 160
ecs.hfc5.6xlarge 24 48 4.5 200
ecs.hfc5.8xlarge 32 64 6 250
ecs.hfg5.large 2 8 1 30
ecs.hfg5.xlarge 4 16 1.5 50
ecs.hfg5.2xlarge 8 32 2 100
ecs.hfg5.3xlarge 12 48 2.5 130
ecs.hfg5.4xlarge 16 64 3 160
ecs.hfg5.6xlarge 24 96 4.5 200
ecs.hfg5.8xlarge 32 128 6 250
ecs.hfg5.14xlarge 56 160 10 400
ecs.gn6v-c8g1.2xlarge 8 32 1 2.5
ecs.gn6v-c8g1.8xlarge 32 128 4 10
ecs.gn6v-c8g1.16xlarge 64 256 8 20
ecs.gn5-c4g1.xlarge 4 30 1 3
ecs.gn5-c8g1.2xlarge 8 60 1 3
ecs.gn5-c4g1.2xlarge 8 60 2 5
ecs.gn5-c8g1.4xlarge 16 120 2 5
ecs.gn5-c28g1.7xlarge 28 112 1 5
ecs.gn5-c8g1.8xlarge 32 240 4 10
ecs.gn5-c28g1.14xlarge 56 224 2 10
ecs.gn5-c8g1.14xlarge 54 480 8 25
ecs.gn5i-c2g1.large 2 8 1 1
ecs.gn5i-c4g1.xlarge 4 16 1 1.5
ecs.gn5i-c8g1.2xlarge 8 32 1 2
ecs.gn5i-c16g1.4xlarge 16 64 1 3
ecs.gn5i-c16g1.8xlarge 32 128 2 6
ecs.gn5i-c28g1.14xlarge 56 224 2 10
ecs.gn4-c4g1.xlarge 4 30 1 3
ecs.gn4-c8g1.2xlarge 8 30 1 3
ecs.gn4.8xlarge 32 48 1 6
ecs.gn4-c4g1.2xlarge 8 60 2 5
ecs.gn4-c8g1.4xlarge 16 60 2 5
ecs.gn4.14xlarge 56 96 2 10
ecs.ga1.xlarge 4 10 0.25 1
ecs.ga1.2xlarge 8 20 0.5 1.5
ecs.ga1.4xlarge 16 40 1 3
ecs.ga1.8xlarge 32 80 2 6
ecs.ga1.14xlarge 56 160 4 10
ecs.f1-c8f1.2xlarge 8 60 NaN 3
ecs.f1-c8f1.4xlarge 16 120 2 5
ecs.f1-c28f1.7xlarge 28 112 NaN 5
ecs.f1-c28f1.14xlarge 56 224 2 10
ecs.f2-c8f1.2xlarge 8 60 NaN 2
ecs.f2-c8f1.4xlarge 16 120 2 5
ecs.f2-c28f1.7xlarge 28 112 NaN 5
ecs.f2-c28f1.14xlarge 56 224 2 10
ecs.f3-c16f1.4xlarge 16 64 1 5
ecs.f3-c16f1.8xlarge 32 128 2 10
ecs.f3-c16f1.16xlarge 64 256 4 20
ecs.ebmhfg5.2xlarge 8 32 6 200
ecs.ebmc4.8xlarge 32 64 10 400
ecs.ebmg5.24xlarge 96 384 10 400
ecs.scch5.16xlarge 64 192 10 450
ecs.sccg5.24xlarge 96 384 10 450
ecs.t5-lc2m1.nano 1 10 6 144
ecs.t5-lc1m1.small 1 10 6 144
ecs.t5-lc1m2.small 1 10 6 144
ecs.t5-lc1m2.large 2 10 12 288
ecs.t5-lc1m4.large 2 10 12 288
ecs.t5-c1m1.large 2 15 18 432
ecs.t5-c1m2.large 2 15 18 432
ecs.t5-c1m4.large 2 15 18 432
ecs.t5-c1m1.xlarge 4 15 36 864
ecs.t5-c1m2.xlarge 4 15 36 864
ecs.t5-c1m4.xlarge 4 15 36 864
ecs.t5-c1m1.2xlarge 8 15 72 1728
ecs.t5-c1m2.2xlarge 8 15 72 1728
ecs.t5-c1m4.2xlarge 8 15 72 1728
ecs.t5-c1m1.4xlarge 16 15 144 3456
ecs.t5-c1m2.4xlarge 16 15 144 3456
ecs.xn4.small 1 1 0.5 5
ecs.n4.small 1 2 0.5 5
ecs.n4.large 2 4 0.5 10
ecs.n4.xlarge 4 8 0.8 15
ecs.n4.2xlarge 8 16 1.2 30
ecs.n4.4xlarge 16 32 2.5 40
ecs.n4.8xlarge 32 64 5 50
ecs.mn4.small 1 4 0.5 5
ecs.mn4.large 2 8 0.5 10
ecs.mn4.xlarge 4 16 0.8 15
ecs.mn4.2xlarge 8 32 1.2 30
ecs.mn4.4xlarge 16 64 2.5 40
ecs.mn4.8xlarge 32 128 5 50
ecs.e4.small 1 8 0.5 5
ecs.e4.large 2 16 0.5 10
ecs.e4.xlarge 4 32 0.8 15
ecs.e4.2xlarge 8 64 1.2 30
ecs.e4.4xlarge 16 128 2.5 40"""
ecs_data = map(lambda x: [x.split(" ")[0], *list(map(lambda y: float(y), x.split(" ")[1:]))], ecss.split("\n"))


def essd_optimizer(nnodes, mem, cpu, nssd, ssd_size, net_tp, net_pps, file_size, client_count, client_iops):
    ssd_iops = min(1800+50*ssd_size/(1024*1024*1024), 1000000)
    ssd_tp = min(120+0.5*ssd_size/(1024*1024*1024), 4000) * 1024*1024

    return optimizer(nnodes, mem, cpu, nssd, ssd_size, ssd_iops, ssd_tp, net_tp, net_pps,
                     4096, 9000, file_size, client_count, client_iops)


def optimizer(nnodes, mem, cpu, nssd, ssd_size, ssd_iops, ssd_tp, net_tp, net_pps, block_size, mtu,
              file_size, client_count, client_iops):

    total_client_iops = client_count * client_iops

    if mem < ssd_size * (32 / file_size):
        return False

    cache_rate = mem / (ssd_size * nssd)

    if file_size / mtu * total_client_iops / nnodes > net_pps:
        return False

    if file_size * total_client_iops / nnodes > net_tp:
        return False

    if file_size / block_size * total_client_iops / nnodes * cache_rate > ssd_iops * nssd:
        return False

    if file_size * total_client_iops / nnodes * cache_rate > ssd_tp * nssd:
        return False

    if file_size * total_client_iops / nnodes / (500 * 1024*1024) > cpu:
        return False
    return True


file_size = 150*1024
client_count = 10
client_iops = 5000
min_price = 100000000
s = 'not found'

selections = []
for nnodes in (4, 8, 16, 32):
    for nssd in (1, 5):
        for ssd_size in range(1, 5):
            # ecs_optimizer(nnodes, nssd, ssd_size*1024*1024*1024*1024, file_size, client_count, client_iops)
            ssd_size *= 1024**4
            for data in ecs_data:
                name = data[0]
                cpu = data[1]
                mem = data[2] * 1024 * 1024 * 1024
                net_tp = data[3] * 1024 * 1024 * 1024 / 8
                net_pps = data[4] * 10000
                ok = essd_optimizer(nnodes, mem, cpu, nssd, ssd_size, net_tp, net_pps, file_size, client_count, client_iops)

                if ok:
                    if name in prices:
                        ssd_price = ssd_size/1024/1024/1024 * 0.8  * nssd
                        node_price = prices[name] * nnodes
                        total_price = node_price + ssd_price
                        s = "%s: x%d, %dxSSD %.2fG price %f" % (name, nnodes, nssd, ssd_size/1024/1024/1024, total_price)
                        selections.append((name, s, total_price))
selections = sorted(selections, key=lambda x: x[2])
for s in selections:
    print(s)


