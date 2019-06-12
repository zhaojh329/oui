<template>
  <div class="dlist">
    <el-tag closable v-for="tag in value" :key="tag" :disable-transitions="false" @close="handleDelDlist(tag)">{{ tag }}</el-tag>
    <el-input v-if="inputVisible" size="small" ref="input" v-model="inputValue" @keyup.enter.native="handleinputConfirm" @blur="handleinputConfirm"></el-input>
    <el-button v-else size="small" @click="showInput">+ {{ $t('Add') }}</el-button>
  </div>
</template>

<script>
export default {
  name: 'UciDlist',
  props: {
    value: {
      type: Array,
      default: () => {
        return [];
      }
    }
  },
  data() {
    return {
      inputVisible: false,
      inputValue: ''
    }
  },
  methods: {
    handleDelDlist(tag) {
      let tags = [...this.value];
      tags.splice(tags.indexOf(tag), 1);
      this.$emit('input', tags);
    },
    showInput() {
      this.inputVisible = true;
      this.$nextTick(() => {
        this.$refs['input'].focus();
      });
    },
    handleinputConfirm() {
      if (this.inputValue) {
        const tags = [...this.value];
        tags.push(this.inputValue);
        this.$emit('input', tags);
        this.inputValue = '';
      }
      this.inputVisible = false;
    }
  }
}
</script>

<style lang="scss" scoped>
.dlist {
  display: flex;
  flex-direction: column;

  > * {
    margin-bottom: 5px;
  }

  .el-button {
    width: 100px;
  }
}
</style>
