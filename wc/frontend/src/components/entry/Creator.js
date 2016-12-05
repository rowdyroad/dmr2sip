import React from 'react'

const Creator = (component, scope, componentProps) => {
  return (props)=> { return React.createElement(component, {...props, ...props[scope], ...componentProps}) };
}

export default Creator;
