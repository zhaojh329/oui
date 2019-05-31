<template>
  <el-input type="textarea" v-model="dmesg" :autosize="true"></el-input>
</template>

<script>
export default {
  data() {
    return {
      dmesg: ''
    }
  },
  mounted() {
    this.$ubus.call('oui.system', 'dmesg').then(r => {
      this.dmesg = r.log;
    });
  }
}
</script>
