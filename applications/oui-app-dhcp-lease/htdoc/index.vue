<template>
  <el-card :header="$t('Active DHCP Leases')">
    <el-table :data="leases">
      <el-table-column prop="hostname" :label="$t('Hostname')" width="180"/>
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
</template>

<script>
export default {
  data() {
    return {
      leases: []
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
    }
  },
  created() {
    this.$timer.create('dhcp', this.getDhcpLeases, { time: 3000, immediate: true, repeat: true })
  }
}
</script>

<i18n src="./locale.json"/>
