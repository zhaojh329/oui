<template>
  <div>
    <el-input v-model="search" size="small" :placeholder="$t('Enter keywords to filter')" style="margin-bottom: 20px; width: 200px;" />
    <el-table :data="syslog.filter(log => !search || log.time.includes(search) || log.msg.includes(search))" v-loading="loading" :element-loading-text="$t('Loading...')">
      <el-table-column :label="$t('Datetime')" prop="datetime" width="220"></el-table-column>
      <el-table-column :label="$t('Facility-syslog')" prop="facility" width="100"></el-table-column>
      <el-table-column :label="$t('Level')" prop="level" width="100"></el-table-column>
      <el-table-column :label="$t('Message')" prop="msg"></el-table-column>
    </el-table>
  </div>
</template>

<script>
export default {
  data() {
    return {
      syslog: [],
      loading: true,
      search: ''
    }
  },
  created() {
    this.$ubus.call('oui.system', 'syslog').then(r => {
      this.syslog = r.log;
      this.loading = false;
    });
  }
}
</script>
