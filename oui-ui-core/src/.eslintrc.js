module.exports = {
  root: true,
  env: {
    node: true
  },
  'extends': [
    'plugin:vue/essential',
    'eslint:recommended'
  ],
  rules: {
    'no-console': process.env.NODE_ENV === 'production' ? 'error' : 'off',
    'no-debugger': process.env.NODE_ENV === 'production' ? 'error' : 'off',
    'linebreak-style': ['error', 'unix'],
    'indent': ['error', 2],
    'quotes': ['error', 'single'],
    'brace-style': 'error',
    'comma-dangle': 'error',
    'comma-spacing': 'error',
    'keyword-spacing': 'error',
    'no-trailing-spaces': 'error',
    'no-unneeded-ternary': 'error',
    'space-before-function-paren': ['error', 'never'],
    'space-infix-ops': ['error', {'int32Hint': false}],
    'arrow-spacing': 'error',
    'no-var': 'error',
    'no-duplicate-imports': 'error',
    'space-before-blocks': 'error',
    'space-in-parens': ["error", 'never'],
    'no-multi-spaces': 'error',
    'eqeqeq': 'error'
  },
  parserOptions: {
    parser: 'babel-eslint'
  }
}
