<template>
  <div class="example">
    <a-input-search :placeholder="$t('example.Please input text')" :enter-button="$t('example.Send')" size="large" @search="send"/>
    <a-input v-model="resp"/>
  </div>
</template>

<script>
export default {
  data () {
    return {
      resp: ''
    }
  },
  methods: {
    send (value) {
      this.$rpc.call('example', 'echo', { text: value }).then(r => {
        this.resp = r.text
      })
    }
  }
}
</script>

<style>
.example {
  width: 500px;
}

.example .ant-input-search {
  margin-bottom: 20px;
}
</style>
