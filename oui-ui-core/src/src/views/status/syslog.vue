<template>
  <el-input type="textarea" v-model="syslog" :autosize="true"></el-input>
</template>

<script>

export default {
  data() {
    return {
      syslog: ''
    }
  },
  mounted() {
    this.$ubus.call('oui.system', 'syslog').then(r => {
      this.syslog = r.log;
    });
  }
}
</script>
