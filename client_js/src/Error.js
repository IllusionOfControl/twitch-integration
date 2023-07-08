const Error = ({error, description}) => {
  return (
    <div>
      <h1>{error}</h1>
      <h3>{description}</h3>
    </div>
  )
}

export default Error;
