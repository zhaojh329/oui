<template>
  <el-button v-if="visible" type="danger" size="mini" @click="del">{{ $t('Delete') }}</el-button>
</template>

<script>
export default {
  name: 'UciSectionDel',
  props: {
    sestion: Object,
    sid: String
  },
  computed: {
    visible() {
      const s = this.sestion;
      return s.addable && s.type && !s.name;
    }
  },
  methods: {
    del() {
      this.sestion.del(this.sid);
    }
  }
}
</script>
