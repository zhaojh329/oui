<template>
  <el-space direction="vertical" fill>
    <el-card :header="$t('Active DHCP Leases')">
      <el-table :data="leases">
        <el-table-column prop="hostname" :label="$t('Hostname')" width="200"/>
        <el-table-column prop="ipaddr" :label="$t('IPv4 address')" width="120"/>
        <el-table-column :label="$t('MAC address')" width="150">
          <template #default="{ row }">
            <span>{{ row.macaddr.toUpperCase() }}</span>
          </template>
        </el-table-column>
        <el-table-column :label="$t('Lease')" width="180">
          <template #default="{ row }">
            <span>{{ formatSecond(row.expire) }}</span>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
    <el-card :header="$t('Active DHCPv6 Leases')">
      <el-table :data="leases6">
        <el-table-column prop="hostname" :label="$t('Hostname')" width="200"/>
        <el-table-column :label="$t('IPv6 address')" width="200">
          <template #default="{ row }">
            <div v-for="a in row.ipaddr" :key="a">{{ a }}</div>
          </template>
        </el-table-column>
        <el-table-column prop="duid" label="DUID" width="250"/>
        <el-table-column :label="$t('Lease')" width="180">
          <template #default="{ row }">
            <span>{{ formatSecond(row.expire) }}</span>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
  </el-space>
</template>

<script>
export default {
  data() {
    return {
      leases: [],
      leases6: []
    }
  },
  methods: {
    formatSecond(second) {
      const days = Math.floor(second / 86400)
      const hours = Math.floor((second % 86400) / 3600)
      const minutes = Math.floor(((second % 86400) % 3600) / 60)
      const seconds = Math.floor(((second % 86400) % 3600) % 60)
      return `${days}d ${hours}h ${minutes}m ${seconds}s`
    },
    getDhcpLeases() {
      this.$oui.call('network', 'dhcp_leases').then(({ leases }) => {
        this.leases = leases
      })
    },
    getDhcpLeases6() {
      let leases6 = []
      this.$oui.ubus('dhcp', 'ipv6leases').then(({ device }) => {
        Object.keys(device).forEach(dev => {
          const leases = device[dev].leases.map(l => {
            return {
              hostname: l.hostname,
              duid: l.duid,
              ipaddr: l['ipv6-addr'].map(a => a.address),
              expire: l.valid
            }
          })
          leases6 = [...leases6, ...leases]
        })

        this.leases6 = leases6
      })
    }
  },
  created() {
    this.$timer.create('dhcp', this.getDhcpLeases, { time: 3000, immediate: true, repeat: true })
    this.$timer.create('dhcp6', this.getDhcpLeases6, { time: 3000, immediate: true, repeat: true })
  }
}
</script>

<i18n src="./locale.json"/>
