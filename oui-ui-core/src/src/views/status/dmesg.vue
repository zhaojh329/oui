<template>
  <div>
    <el-input v-model="search" size="small" :placeholder="$t('Enter keywords to filter')" style="margin-bottom: 20px; width: 200px;" />
    <el-table :data="dmesg.filter(log => !search || log.time.includes(search) || log.msg.includes(search))" v-loading="loading" :element-loading-text="$t('Loading...')">
      <el-table-column :label="$t('Time')" prop="time" width="150"></el-table-column>
      <el-table-column :label="$t('Message')" prop="msg"></el-table-column>
    </el-table>
  </div>
</template>

<script>
export default {
  data() {
    return {
      dmesg: [],
      loading: true,
      search: ''
    }
  },
  created() {
    this.$ubus.call('oui.system', 'dmesg').then(r => {
      this.dmesg = r.log;
      this.loading = false;
    });
  }
}
</script>
