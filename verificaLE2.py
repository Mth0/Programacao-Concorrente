#Define funcoes para verificar a logica de execucao de uma aplicacao leitor/escritor

class LE:
	def __init__(self):
		self.escritores = 0
		self.leitores = 0
		self.escrEsperando = 0

	def leitorBloqueado(self, id, linha):
		'''Recebe o id do leitor. Verifica se a decisao de bloqueio esta correta.'''
		if(self.escritores==0 and self.escritorEscrevendo == 0):
			print("linha "+ str(linha) + " ----> ERRO:  leitor " + str(id) + " bloqueado quando nao ha escritores!")

	def escritorBloqueado(self, id, linha):
		'''Recebe o id do escritor. Verifica se a decisao de bloqueio esta correta.'''
		self.escrEsperando = 1
		if(self.escritores==0) and (self.leitores==0):
			print("linha "+ str(linha) + " ----> ERRO:  escritor " + str(id) + " bloqueado quando nao ha escritores nem leitores!")

	def leitorLendo(self, id, linha):
		'''Recebe o id do leitor, verifica se pode ler e registra que esta lendo.'''
		if(self.escritores>0 or self.escrEsperando == 1):
			print("linha "+ str(linha) + " ----> ERRO:  leitor " + str(id) + " esta lendo quando ha escritor escrevendo ou esperando!")
		self.leitores+=1

	def escritorEscrevendo(self, id, linha):
		self.escrEsperando = 0
		'''Recebe o id do escritor, verifica se pode escrever e registra que esta escrevendo'''
		if(self.escritores>0 or self.leitores>0):
			print("linha "+ str(linha) + " ----> ERRO:  escritor " + str(id) + " esta escrevendo quando ha outro escritor ou leitores!")
		self.escritores+=1

	def leitorSaindo(self, id, linha):
		'''Recebe o id do leitor e registra que terminou a leitura.'''
		self.leitores-=1

	def escritorSaindo(self, id, linha):
		'''Recebe o id do escritor e registra que terminou a leitura.'''
		self.escritores-=1

##acrescentar uma sequencia de escritas e leituras
